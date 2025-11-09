#include "MotArda/Engine.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

//-------Triangle example data-----------------
const int ammountPoints = 3;
struct Vertex {
    float x, y;
    float r, g, b;
};

Vertex vertex[ammountPoints] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {  0.f,  0.6f, 0.f, 0.f, 1.f }
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
    eng.windowSetSwapInterval();

    // --- Vectores de uniforms y atributos ---
    std::vector<Window::UniformAttrib> uniforms = {
        {"MVP", -1, Window::UniformTypes::Mat4, nullptr}
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "vPos", 2, offsetof(Vertex, x) },
        { "vCol", 3, offsetof(Vertex, r) }
    };

    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/triangle_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/triangle_fragment.txt");

    eng.windowOpenglSetup(
        vertex,
        vertex_shader,
        fragment_shader,
        uniforms,
        attributes,
        sizeof(Vertex),
        ammountPoints
    );

    glm::mat4x4 m, p, mvp;
    uniforms[0].values = glm::value_ptr(mvp);

    float ratio = eng.windowGetSizeRatio();

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        m = glm::mat4(1.f);
        m = glm::rotate(m, (float)eng.windowGetTimer(), glm::vec3(0.f, 0.f, 1.f));
        p = glm::ortho(-ratio, ratio, -1.f, 1.f, -1.f, 1.f);
        mvp = p * m;


        eng.windowOpenglSetUniformsValues(uniforms);
        eng.windowOpenglProgramUniformDraw(ammountPoints);

        eng.windowEndFrame();
    }

    return 0;
}
