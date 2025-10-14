#include "MotArda/Engine.hpp"

#include <memory>
#include "../deps/linmath.h-master/linmath.h"

//Ref: https://www.glfw.org/docs/3.3/quick.html#quick_example
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
    fprintf(stderr, "Glfw error: %s\n", description);
}

//---------------------------------------------


int MTRD::main() {

	// Create a blank window
	auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda triangle");

	// Check if not null
    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        eng.windowSetErrorCallback(error_callback);
        eng.windowCreateContext();

        //this is default set to 1
        eng.windowSetSwapInterval();

        //opengl functions
        eng.windowOpenglSetup(
            vertex,
            vertex_shader_text,
            fragment_shader_text,
            "MVP",
            "vPos",
            "vCol",
            sizeof(vertex[0])
        );


        mat4x4 m, p, mvp;
        float ratio = eng.windowGetSizeRatio();

        float yMov = 0;
        float xMov = 0;

        while (!eng.windowShouldClose()) {

            eng.windowOpenglViewportAndClear();

            float speed = 0.01f;

            if (eng.inputIsKeyPressed(Input::Keyboard::D)) {
                xMov += 1 * speed;
            }
            else if (eng.inputIsKeyPressed(Input::Keyboard::A)) {
                xMov += -1 * speed;
            }

            if (eng.inputIsKeyPressed(Input::Keyboard::S)) {
                yMov += -1 * speed;
            }
            else if (eng.inputIsKeyPressed(Input::Keyboard::W)) {
                yMov += 1 * speed;
            }

            mat4x4_identity(m);
            mat4x4_rotate_Z(m, m, (float)eng.windowGetTimer());
            mat4x4_translate(m, xMov, yMov, 0);
            mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            mat4x4_mul(mvp, p, m);

            eng.windowOpenglProgramUniformDraw((const GLfloat*) mvp);

            eng.windowEndFrame();
        }

        return 0;
    }

    return 1;
}