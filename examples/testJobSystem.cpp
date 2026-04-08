#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>

#include <memory>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include "MotArda/common/Logger.hpp"
#include <MotArda/win64/Systems/RenderSystem.hpp>


static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    // --- Create Logger ---
    MTRD::Logger::init("testJobSystem", MTRD::Logger::Level::Debug);

    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();
    // --- *** ---

    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0.f, 1.f, 5.f));
    camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
    // --- *** ---

    // --- Setup Engine ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Base);
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = { "12140_Skull_v3_L2.obj" };
    std::atomic<bool> objsLoaded = false;

    std::vector<ObjItem> ObjList;
    ObjList.push_back(ObjItem());

    // --- Ecs ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t entity = ecs.AddEntity();
    // --- *** ---

    // --- Drawable transforms additions ---
    float ratio = eng.windowGetSizeRatio();
    // The speed at which the object will move and scale
    float movSpeed = 0.05f;
    float scaSpeed = 0.001f;
    float scale = 0.025f;

    // This bool is used to change the displayed mesh
    bool needChangeObj = false;
    int objIndex = 1;

    bool firstTime = true;
    // --- *** ---

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(entity);
    t->position = glm::vec3(0.0f);
    t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.05f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(entity);


    // async obj load
    eng.enqueueTask([&]() {
        ObjList = eng.loadObjs(objsRoutes);

        objsLoaded = true;
        }
    );

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (!objsLoaded) {
            //printf("Cargando maya...\n");
            MTRD::Logger::debug("Cargando maya...\n");
            eng.windowEndFrame();
            continue;

        }
        else if (firstTime) {
            firstTime = false;
            eng.windowLoadAllMaterials(ObjList);

            r->meshes_ = &ObjList[0].meshes;
            r->materials_ = &ObjList[0].materials;
        }


        // --- Input to move camera ---
        if (eng.inputIsKeyPressed(Input::Keyboard::W)) camera.moveForward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::S)) camera.moveBackward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) camera.moveLeft(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) camera.moveRight(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::E)) camera.moveUp(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::Q)) camera.moveDown(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::R)) camera.rotate(10.0f, 0.0f);
        if (eng.inputIsKeyPressed(Input::Keyboard::T)) camera.rotate(-10.0f, 0.0f);
        // --- *** ---

        if (eng.inputIsKeyDown(Input::Keyboard::C)) {
            needChangeObj = true;
            objIndex = (objIndex + 1) % 3;
        }
        if (eng.inputIsKeyDown(Input::Keyboard::V)) {
            needChangeObj = true;
            objIndex = (objIndex + 2) % 3;
        }

        // --- Input to scale object ---
        if (eng.inputIsKeyPressed(Input::Keyboard::Z)) t->scale -= scaSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::X)) t->scale += scaSpeed;
        // --- *** ---
        
        eng.RenderScene();

        // --- Cambiar objeto si es necesario ---
        if (needChangeObj) {

            t->rotation = glm::vec3(0.0f);
            switch (objIndex) {
            case 0:
                objsRoutes = { "86jfmjiufzv2.obj" };
                t->scale = glm::vec3(0.0001f);
                scaSpeed = 0.0001f;
                break;
            case 1:
                objsRoutes = { "12140_Skull_v3_L2.obj" };
                t->scale = glm::vec3(0.025f);
                scaSpeed = 0.001f;
                t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
                break;
            case 2:
                objsRoutes = { "indoor_plant_02.obj" };
                t->scale = glm::vec3(0.1f);
                scaSpeed = 0.01f;
                break;
            }

            ObjList.clear();
            objsLoaded = false;

            eng.enqueueTask([&]() {
                ObjList = eng.loadObjs(objsRoutes);
                objsLoaded = true;
                });

            while (!objsLoaded) {
                eng.windowEndFrame();
                //printf("Cargando maya %d...\n", objIndex);
                MTRD::Logger::debug("Cargando maya {}...\n", objIndex);


                eng.windowInitFrame();
            }

            //printf("Maya %d cargada\n", objIndex);
            MTRD::Logger::debug("Maya cargada\n");

            r->meshes_ = &ObjList[0].meshes;
            r->materials_ = &ObjList[0].materials;

            eng.windowLoadAllMaterials(ObjList);

            needChangeObj = false;
        }
        eng.windowEndFrame();
    }
    
    MTRD::Logger::shutdown();

    return 0;
}
