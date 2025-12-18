#include "MotArda/Engine.hpp"
#include "MotArda/ObjLoader.hpp"
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>

#include <memory>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/Systems.hpp>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();
    // --- *** ---

    // --- Create window ---
    eng.windowSetDebugMode(true);
    eng.windowSetErrorCallback(error_callback);
    eng.windowCreateContext();
    eng.windowSetSwapInterval(1);
    // --- *** ---

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = { "12140_Skull_v3_L2.obj" };

    std::atomic<bool> objsLoaded = false;
    std::vector<MTRD::Window::ObjItem> objItemList;

    // async obj load
    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );

    // --- Systems ---
    Systems systems;
    // --- *** ---

    // --- Ecs ---
    ECSManager ecs;
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
    MTRD::Transform* t;
    MTRD::Render* r;
    MTRD::Movement* m;
    glm::mat4x4 vp, model;
    // --- *** ---

    // --- Setup uniforms ---
    std::vector<Window::UniformAttrib> uniforms = {
        {"VP", -1, Window::UniformTypes::Mat4, glm::value_ptr(vp)},
        {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "position", 3, offsetof(Vertex, position) },
        { "uv", 2, offsetof(Vertex, uv) },
        { "normal", 3, offsetof(Vertex, normal) }
    };
    // --- *** ---

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
        else if (firstTime) {
            firstTime = false;

            eng.windowLoadAllMaterials(objItemList);
            if (objItemList.size() == 0) return 1;
            // --- *** ---

            // --- Create drawable entitys ---
            ecs.AddComponentType<MTRD::Transform>();
            ecs.AddComponentType<MTRD::Render>();

            size_t entity = ecs.AddEntity();

            t = ecs.AddComponent<MTRD::Transform>(entity);
            t->position = glm::vec3(0.0f);
            t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
            t->angleRotationRadians = -1;
            t->scale = glm::vec3(0.05f);

            r = ecs.AddComponent<MTRD::Render>(entity);
            r->shapes = &objItemList[0].shapes;
            r->materials = &objItemList[0].materials;
            // --- *** ---

            // --- Load shaders ---
            const char* vertex_shader = eng.loadShaderFile("../assets/shaders/textured_obj_vertex.txt");
            const char* fragment_shader = eng.loadShaderFile("../assets/shaders/textured_obj_fragment.txt");
            // --- *** ---

            // --- Setup Window ---
            eng.windowOpenglSetup(
                ecs.GetComponentList<Render>(),
                vertex_shader,
                fragment_shader,
                uniforms,
                attributes
            );
            // --- *** ---
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
         
        // --- update vp ---
        vp = camera.getViewProj();
        // --- *** ---

        // --- Setup uniforms and draw ---
        systems.RunRenderSystem(ecs, eng, uniforms, model);
        // --- *** ---

        eng.windowEndFrame();


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

            objItemList.clear();
            objsLoaded = false;

            eng.enqueueTask([&]() {
                objItemList = eng.loadObjs(objsRoutes);
                objsLoaded = true;
                });

            while (!objsLoaded) {
                eng.windowInitFrame();
                printf("Cargando maya %d...\n", objIndex);
                eng.windowEndFrame();
            }

            printf("Maya %d cargada\n", objIndex);

            r->shapes = &objItemList[0].shapes;
            r->materials = &objItemList[0].materials;

            eng.updateVertexBuffers(ecs.GetComponentList<Render>(), uniforms, attributes);
            eng.windowLoadAllMaterials(objItemList);

            needChangeObj = false;
        }
    }

    return 0;
}
