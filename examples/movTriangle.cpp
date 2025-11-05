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
    { -0.3f, -0.2f, 1.f, 0.f, 0.f },
    {  0.f,   0.3f, 0.f, 0.f, 1.f },
    {  0.3f, -0.2f, 0.f, 1.f, 0.f },
};

static const char* vertex_shader_text =
"#version 460 core\n"
"uniform mat4 MVP;\n"
"in vec2 vPos;\n"
"in vec3 vCol;\n"
"out vec3 color;\n"
"void main() {\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 460 core\n"
"in vec3 color;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    outColor  = vec4(color, 1.0);\n"
"}\n";

//-------Triangle example functions------------
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw error: %s\n", description);
}
//---------------------------------------------

int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda triangle");

    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    eng.windowSetDebugMode(true);
    eng.windowSetErrorCallback(error_callback);
    eng.windowCreateContext();
    eng.windowSetSwapInterval();

    // --- Vectores de uniforms y atributos ---
    std::vector<const char*> uniforms = { "MVP" };
    std::vector<Window::VertexAttrib> attributes = {
        { "vPos", 2, offsetof(Vertex, x) },
        { "vCol", 3, offsetof(Vertex, r) }
    };

    eng.windowOpenglSetup(
        vertex,
        vertex_shader_text,
        fragment_shader_text,
        uniforms,
        attributes,
        sizeof(Vertex),
        ammountPoints
    );

    glm::mat4x4 m, p, mvp;
    float ratio = eng.windowGetSizeRatio();

    float movSpeed = 0.01f;
    float xPos = 0, yPos = 0;
    float rotSpeed = 0.01f, rotationAngle = 0.f;
    float scaSpeed = 0.1f, scale = 1.f;

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (eng.inputIsKeyPressed(Input::Keyboard::D)) xPos += movSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::A)) xPos -= movSpeed;

        if (eng.inputIsKeyPressed(Input::Keyboard::S)) yPos -= movSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::W)) yPos += movSpeed;

        if (eng.inputIsKeyPressed(Input::Keyboard::Q)) rotationAngle -= rotSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::E)) rotationAngle += rotSpeed;

        if (eng.inputIsKeyPressed(Input::Keyboard::Z)) scale -= scaSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::X)) scale += scaSpeed;

        if (eng.inputIsKeyDown(Input::Keyboard::F)) rotationAngle -= rotSpeed * 100;
        else if (eng.inputIsKeyUp(Input::Keyboard::F)) rotationAngle += rotSpeed * 100;

        m = glm::mat4(1.f);
        m = glm::translate(m, { xPos, yPos, -1.f });
        m = glm::rotate(m, rotationAngle, glm::vec3(0.f, 0.f, 1.f));
        m = glm::scale(m, { scale, scale, scale });
        p = glm::perspective(90.f, ratio, 0.1f, 10000.f);
        mvp = p * m;

        // MVP es el primer uniform del vector
        eng.windowOpenglProgramUniformDraw(glm::value_ptr(mvp), ammountPoints);

        eng.windowEndFrame();
    }

    return 0;
}
