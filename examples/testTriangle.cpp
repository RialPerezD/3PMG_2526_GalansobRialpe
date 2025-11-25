#include "MotArda/Engine.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <MotArda/Camera.hpp>
#include <MotArda/Ecs.hpp>

//-------Triangle example data-----------------
const int ammountPoints = 3;
struct Vertexes {
    float x, y, z;
    float u, v;
    float r, g, b;
};

Vertexes vertex[ammountPoints] =
{
    { -6.f, -4.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f },
    {  6.f, -4.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f },
    {  0.f,  6.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f }
};

//---------------------------------------------


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
    eng.windowSetSwapInterval(1);

    // --- Create drawable entitys ---
    std::unordered_map<unsigned long, std::string> entityIdList;
    ECSManager ecs;
    ecs.AddComponentType<MTRD::Transform>();
    ecs.AddComponentType<MTRD::Render>();

    unsigned long entity = ecs.AddEntity();

    MTRD::Transform* t = ecs.AddComponent<MTRD::Transform>(entity);
    t->position = glm::vec3(1.0f, 0.0f, 0.0f);
    t->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    t->scale = glm::vec3(1.0f, 1.0f, 1.0f);

    MTRD::Render* r = ecs.AddComponent<MTRD::Render>(entity);
    r->shapes->push_back(Shape());


    for (int i = 0; i < ammountPoints; i++) {
        Vertex vtx = Vertex();
        vtx.position = {vertex[i].x, vertex[i].y, vertex[i].z};
        vtx.normal = { vertex[i].r, vertex[i].g, vertex[i].b};

        r->shapes->at(0).vertices.push_back(vtx);
    }

    std::vector<Render*> redners;
    for (std::pair<size_t, Render> element : ecs.GetComponentList<Render>()) {
        redners.push_back(&element.second);
    }
    // --- *** ---

    // --- Load shaders ---
    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/triangle_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/triangle_fragment.txt");
    // --- *** ---

    // --- Setup uniforms ---
    glm::mat4x4 mvp, model;
    std::vector<Window::UniformAttrib> uniforms = {
        {"MVP", -1, Window::UniformTypes::Mat4, glm::value_ptr(mvp)}
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "position", 3, offsetof(Vertexes, x) },
        { "voido", 3, offsetof(Vertexes, u) },
        { "triCol", 3, offsetof(Vertexes, r) }
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

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

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
