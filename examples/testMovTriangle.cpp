#include "MotArda/Engine.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda triangle");

    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    eng.windowSetDebugMode(true);
    eng.windowSetErrorCallback(error_callback);
    eng.windowCreateContext();
    eng.windowSetSwapInterval();

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
    Shape shape = Shape();
    shape.vertices.push_back(
        {
            { 1.0f,  0.0f, 0.f },
            { 0.0f,  0.0f },
            { 1.0f,  0.0f, 0.f }
        });
    shape.vertices.push_back(
        {
            { 0.0f,  1.5f, 0.f},
            { 0.0f,  0.0f},
            { 0.0f,  1.0f, 0.f}
        });
    shape.vertices.push_back(
        {
            {-1.0f,  0.0f, 0.f},
            { 0.0f,  0.0f},
            { 0.0f,  0.0f, 1.f}
        });
    r->shapes->push_back(shape);
    // --- *** ---

    // --- Vectores de uniforms y atributos ---
    glm::mat4x4 mvp, model, p;

    std::vector<Window::UniformAttrib> uniforms = {
        {"MVP", -1, Window::UniformTypes::Mat4, glm::value_ptr(mvp)}
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "vPos", 3, offsetof(Vertex, position.x) },
        { "vCol", 3, offsetof(Vertex, normal.x) }
    };
    // --- *** --- 
    
    // --- Load shaders ---
    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/triangle_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/triangle_fragment.txt");
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

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        camera.updateAll();
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

        model = glm::mat4(1.f);
        glm::mat4x4 vp = camera.getViewProj();
        mvp = vp * model;


        eng.windowOpenglSetUniformsValues(uniforms);
        eng.windowOpenglProgramUniformDrawRender(*r);

        eng.windowEndFrame();
    }

    return 0;
}
