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

    // --- Rand seed ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // --- *** ---

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
    std::vector <const char*> objsRoutes = {
        "indoor_plant_02.obj",
        "12140_Skull_v3_L2.obj"
    };

    std::atomic<bool> objsLoaded = false;
    std::vector<MTRD::Window::ObjItem> objItemList;
    // --- *** ---


    // async obj load
    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );

    while (!objsLoaded) {
        eng.windowInitFrame();
        printf("Cargando maya...\n");
        eng.windowEndFrame();
    }

    eng.windowLoadAllMaterials(objItemList);

    if (objItemList.size() == 0) return 1;
    // --- *** ---


    // --- Create drawable entitys ---
    ECSManager ecs;
    ecs.AddComponentType<MTRD::Transform>();
    ecs.AddComponentType<MTRD::Render>();
    ecs.AddComponentType<MTRD::Movement>();

    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            unsigned long entity = ecs.AddEntity();

            float scl = 0.01f + rand() / (float)RAND_MAX * 0.06f;
            MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(entity);
            t->position = glm::vec3(-3.0f + (y * 0.6), -2.0f + (x * 0.4f), 0.0f);
            t->rotation = glm::vec3(0.0f);
            t->scale = glm::vec3(scl);

            MTRD::Render* r = ecs.AddComponent<MTRD::Render>(entity);
            r->shapes = &objItemList[0].shapes;
            r->materials = &objItemList[0].materials;

            MTRD::Movement* m = ecs.AddComponent<MTRD::Movement>(entity);
            m->position = glm::vec3(std::rand() % 3 - 1, std::rand() % 3 - 1, 0);
            m->rotation = glm::vec3(std::rand() % 3 - 1, std::rand() % 3 - 1, std::rand() % 3 - 1);
            m->scale = glm::vec3(0.0f);
        }
    }


    unsigned long player = ecs.AddEntity();

    MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(player);
    t->position = glm::vec3(0);
    t->rotation = glm::vec3(1,0,0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.02f);

    MTRD::Render* r = ecs.AddComponent<MTRD::Render>(player);
    r->shapes = &objItemList[1].shapes;
    r->materials = &objItemList[1].materials;

    MTRD::Movement* m = ecs.AddComponent<MTRD::Movement>(player);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;
    // --- *** ---

    // --- Systems ---
    Systems systems;
    // --- *** ---


    // --- Load shaders ---
    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/textured_obj_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/textured_obj_fragment.txt");
    // --- *** ---


    // --- Setup uniforms ---
    glm::mat4x4 vp, model;
    //empacar
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


    // --- Setup Window ---
    eng.windowOpenglSetup(
        ecs.GetComponentList<Render>(),
        vertex_shader,
        fragment_shader,
        uniforms,
        attributes
    );
    //la ventana no gestio el shader, el rebder es el
    // --- *** ---


    // --- Drawable transform additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
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

    //eng deveria decirme cuanto ha tardado el ultimo frame

    float frameTime = 0;

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();


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


        // --- Input to move player skull ---
        if (eng.inputIsKeyPressed(Input::Keyboard::I)) m->position.y += 0.1f;
        if (eng.inputIsKeyPressed(Input::Keyboard::K)) m->position.y -= 0.1f;
        if (eng.inputIsKeyPressed(Input::Keyboard::L)) m->position.x += 0.1f;
        if (eng.inputIsKeyPressed(Input::Keyboard::J)) m->position.x -= 0.1f;
        if (eng.inputIsKeyPressed(Input::Keyboard::U)) m->angleRotationRadians += 0.05f;
        if (eng.inputIsKeyPressed(Input::Keyboard::O)) m->angleRotationRadians -= 0.05f;
        if (eng.inputIsKeyPressed(Input::Keyboard::N)) m->scale += glm::vec3(0.005f);
        if (eng.inputIsKeyPressed(Input::Keyboard::M)) m->scale -= glm::vec3(0.005f);
        // --- *** ---


        // --- Item Movement ---
        glm::vec3 itemMov = glm::vec3(0.f, 0.f, 0.f);
        if (eng.inputIsKeyPressed(Input::Keyboard::I)) {
            itemMov.x = 1;
        }else if(eng.inputIsKeyPressed(Input::Keyboard::K)){
            itemMov.x = -1;
        }
        if (eng.inputIsKeyPressed(Input::Keyboard::J)) {
            itemMov.y = 1;
        }
        else if (eng.inputIsKeyPressed(Input::Keyboard::L)) {
            itemMov.y = -1;
        }
        // --- *** ---


        // --- update vp ---
        vp = camera.getViewProj();
        // --- *** ---


        // --- Setup uniforms and draw ---
        systems.RunRenderSystemWithTraslations(ecs, eng, uniforms, model);
        // --- *** ---

        eng.windowEndFrame();

        frameTime = eng.windowGetLastFrameTime();
        //printf("Last frame time: %.4f secs\n", frameTime);
    }

    return 0;
}
