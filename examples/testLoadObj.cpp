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

    unsigned long entity = ecs.AddEntity();

    MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(entity);
    t->position = glm::vec3(0.0f);
    t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.05f);

    MTRD::Render* r = ecs.AddComponent<MTRD::Render>(entity);
    r->shapes = &objItemList[0].shapes;
    r->materials = &objItemList[0].materials;
    // --- *** ---

    MTRD::Transform* asd = ecs.GetComponent<MTRD::Transform>(entity);

    // --- Systems ---
    Systems systems;
    // --- *** ---

    // --- Load shaders ---
    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/textured_obj_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/textured_obj_fragment.txt");
    // --- *** ---

    // --- Setup uniforms ---
    glm::mat4x4 vp, model;

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
    // --- *** ---

    // --- Drawable transforms additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    float scaSpeed = 0.01f;
    float scale = 0.1f;

    scale = 0.025f; scaSpeed = 0.001f;

    bool needChangeObj = false;
    int objIndex = 0;
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
    }

    return 0;
}
