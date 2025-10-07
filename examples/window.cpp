#include "MotArda/Engine.hpp"

#include <memory>


//-------Triangle example data-----------------
static const struct
{
    float x, y;
    float r, g, b;
} vertex[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
    "#version 110\n"
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";

static const char* fragment_shader_text =
    "#version 110\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";
//---------------------------------------------
//-------Triangle example functions------------
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//---------------------------------------------


int MTRD::main() {

	// Create a blank window
	auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda window");

	// Check if not null
    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        eng.windowSetErrorCallback(error_callback);
        eng.windowCreateContext();

        //this is default set to 1
        eng.windowSetSwapInterval();

        //opengl functions
        eng.windowOpenglGenerateBuffers(vertex);
        eng.windowOpenglGenerateVertexShaders(vertex_shader_text);
        eng.windowOpenglGenerateFragmentShaders(vertex_shader_text);
        eng.windowOpenglCreateProgram();

        while (!eng.windowShouldClose()) {
            eng.windowEndFrame();
        }

        return 0;
    }

    return 1;
}