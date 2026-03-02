#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>

#include <memory>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/win64/Systems/RenderSystem.hpp>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();
    // --- *** ---

    eng.windowSetErrorCallback(error_callback);

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = { "12140_Skull_v3_L2.obj" };

    std::atomic<bool> objsLoaded = false;

    std::vector<ObjItem> ObjList;
    ObjList.push_back(ObjItem());


    // --- Ecs ---
    ECSManager ecs;
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t entity = ecs.AddEntity();
    // --- *** ---

    // --- Drawable transforms additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    float scaSpeed = 0.01f;
    float scale = 0.1f;

    scale = 0.025f; scaSpeed = 0.001f;

    bool needChangeObj = false;
    int objIndex = 1;
    bool firstTime = true;
    glm::mat4x4 vp, model;
    // --- *** ---

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(entity);
    t->position = glm::vec3(0.0f);
    t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.05f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(entity);
    r->meshes_ = &ObjList[0].meshes;
    r->materials_ = &ObjList[0].materials;

    // async obj load
    eng.enqueueTask([&]() {
        ObjList = eng.loadObjs(objsRoutes);

        eng.windowLoadAllMaterials(ObjList);

        r->meshes_ = &ObjList[0].meshes;
        r->materials_ = &ObjList[0].materials;

        objsLoaded = true;
        }
    );

    // --- Camera ---
    MTRD::Camera camera(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::radians(45.f),
        ratio,
        0.1f,
        100.f
    );

    RenderSystem renderSystem = RenderSystem(vp, model);
    eng.windowLoadAllMaterials(ObjList);

    if (ObjList.size() == 0) return 1;


    camera.updateAll();
    // --- *** ---

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (!objsLoaded) {
            printf("Cargando maya...\n");
            eng.windowEndFrame();
            continue;

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

        vp = camera.getViewProj();

        // --- update vp ---
        renderSystem.Render(
            ecs,
            ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>(),
            model,
            true
        );
        // --- *** ---


        eng.windowEndFrame();
    }

    return 0;
}