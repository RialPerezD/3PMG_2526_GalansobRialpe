#include "MotArda/Engine.hpp"
#include "MotArda/ObjLoader.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

// Shader example
static const char* vertex_shader_text =
"#version 460 core\n"
"uniform mat4 MVP;\n"
"in vec3 position;\n"
"in vec3 uv;\n"
"in vec3 normal;\n"
"out vec3 fragColor;\n"
"void main() {\n"
"    gl_Position = MVP * vec4(position, 1.0);\n"
"    fragColor = uv * 0.5 + 0.5;\n"
"}\n";

static const char* fragment_shader_text =
"#version 460 core\n"
"in vec3 fragColor;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    outColor = vec4(fragColor, 1.0);\n"
"}\n";

// Error callback
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw error: %s\n", description);
}

int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    auto maybeObjLoader = ObjLoader::loadObj("../assets/example.obj");
    if (!maybeObjLoader.has_value()) return 1;

    ObjLoader objLoader = maybeObjLoader.value();
    std::vector<Vertex> vertex = objLoader.getVertices();
    const void* vertexBuffer = static_cast<const void*>(vertex.data());

    eng.windowSetDebugMode(true);
    eng.windowSetErrorCallback(error_callback);
    eng.windowCreateContext();
    eng.windowSetSwapInterval();

    // --- Vectores de uniforms y atributos ---
    std::vector<const char*> uniforms = { "MVP" };
    std::vector<Window::VertexAttrib> attributes = {
        { "position", 3, offsetof(Vertex, position) }, // glm::vec3 -> 3 floats
        { "uv", 2, offsetof(Vertex, uv) },             // glm::vec2 -> 2 floats
        { "normal", 3, offsetof(Vertex, normal) }      // glm::vec3 -> 3 floats
    };


    eng.windowOpenglSetup(
        vertexBuffer,
        vertex_shader_text,
        fragment_shader_text,
        uniforms,
        attributes,
        sizeof(Vertex),
        vertex.size()
    );

    glm::mat4x4 m, p, mvp;
    float ratio = eng.windowGetSizeRatio();

    float movSpeed = 0.01f;
    float xPos = 0, yPos = -1;
    float rotSpeed = 0.01f, rotationAngle = 0.f;
    float scaSpeed = 0.01f, scale = 0.25f;

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
        m = glm::scale(m, { scale, scale, scale });
        m = glm::translate(m, { xPos, yPos, 0.f });
        m = glm::rotate(m, rotationAngle, glm::vec3(0.f, 1.f, 0.f));
        p = glm::ortho(-ratio, ratio, -1.f, 1.f, -1.f, 1.f);
        mvp = p * m;

        // MVP es el primer uniform del vector
        eng.windowOpenglProgramUniformDraw(glm::value_ptr(mvp), vertex.size());

        eng.windowEndFrame();
    }

    return 0;
}
