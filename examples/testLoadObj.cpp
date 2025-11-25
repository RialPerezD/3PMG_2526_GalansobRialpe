#include "MotArda/Engine.hpp"
#include "MotArda/ObjLoader.hpp"
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>

#include <memory>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

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
    std::vector <const char*> objsRoutes = { "indoor_plant_02.obj" };

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
    std::unordered_map<unsigned long, std::string> entityIdList;
    ECSManager ecs;
    ecs.AddComponentType<MTRD::Transform>();
    ecs.AddComponentType<MTRD::Render>();

    unsigned long entity = ecs.AddEntity();
    entityIdList[entity] = "Entidad 1";

    MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(entity);
    t->position = glm::vec3(1.0f, 0.0f, 0.0f);
    t->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    t->scale = glm::vec3(1.0f, 1.0f, 1.0f);

    MTRD::Render* r = ecs.AddComponent<MTRD::Render>(entity);
    r->shapes = &objItemList[0].shapes;
    r->materials = &objItemList[0].materials;

    std::vector<Render*> redners;
    for (std::pair<size_t, Render> element : ecs.GetComponentList<Render>()) {
        redners.push_back(&element.second);
    }
    // --- *** ---

    // --- Load shaders ---
    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/textured_obj_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/textured_obj_fragment.txt");
    // --- *** ---

    // --- Setup uniforms ---
    glm::mat4x4 mvp, model;

    std::vector<Window::UniformAttrib> uniforms = {
        {"MVP", -1, Window::UniformTypes::Mat4, glm::value_ptr(mvp)},
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
        redners,
        vertex_shader,
        fragment_shader,
        uniforms,
        attributes
    );
    // --- *** ---

    // --- Drawable transform additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    float scaSpeed = 0.01f;
    float scale = 0.1f;
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


        // --- Input to scale object ---
        if (eng.inputIsKeyPressed(Input::Keyboard::Z)) scale -= scaSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::X)) scale += scaSpeed;
        // --- Load shaders ---

        // --- create mvp ---
        model = glm::mat4(1.f);
        model = glm::scale(model, { scale, scale, scale });

        mvp = camera.getProjection() * camera.getView() * model;
        // --- *** ---

        // --- Setup uniforms and draw ---
        eng.windowOpenglSetUniformsValues(uniforms);
        eng.windowOpenglProgramUniformDrawRender(redners);
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}
