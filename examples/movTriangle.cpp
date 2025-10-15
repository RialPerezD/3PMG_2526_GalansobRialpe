#include "MotArda/Engine.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

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

        glm::mat4x4 m, p, mvp;
        float ratio = eng.windowGetSizeRatio();

        float movSpeed = 0.01f;
        float xPos = 0;
        float yPos = 0;

        float rotSpeed = 0.01f;
        float rotationAngle = 0.0f;

        float scaSpeed = 0.01f;
        float scale = 1;

        while (!eng.windowShouldClose()) {

            eng.windowOpenglViewportAndClear();


            if (eng.inputIsKeyPressed(Input::Keyboard::D)) {
                xPos += 1 * movSpeed;
            }
            else if (eng.inputIsKeyPressed(Input::Keyboard::A)) {
                xPos += -1 * movSpeed;
            }

            if (eng.inputIsKeyPressed(Input::Keyboard::S)) {
                yPos += -1 * movSpeed;
            }
            else if (eng.inputIsKeyPressed(Input::Keyboard::W)) {
                yPos += 1 * movSpeed;
            }

            if (eng.inputIsKeyPressed(Input::Keyboard::Q)) {
                float res = rotationAngle -= 1 * rotSpeed;
                if (res <= 0) {
                    rotationAngle = 360 + res;
                }
                else {
                    rotationAngle = res;
                }
            } else if(eng.inputIsKeyPressed(Input::Keyboard::E)){
                float res = rotationAngle += 1 * rotSpeed;
                if (res >= 360) {
                    rotationAngle = res - 360;
                }
                else {
                    rotationAngle = res;
                }
            }

            if (eng.inputIsKeyPressed(Input::Keyboard::Z)) {
                scale += scaSpeed;
            }
            else if (eng.inputIsKeyPressed(Input::Keyboard::X)) {
                scale -= scaSpeed;
            }

            float a = (float)eng.windowGetTimer();
            
            m = glm::mat4(1.0f);
            m = glm::scale(m, {scale, scale, scale});
            m = glm::translate(m, {xPos, yPos, 0});
            m = glm::rotate(m, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            p = glm::ortho(-ratio, ratio, -1.0f, 1.0f, -1.0f, 1.0f);
            mvp = p * m;

            eng.windowOpenglProgramUniformDraw(glm::value_ptr(mvp));

            eng.windowEndFrame();
        }

        return 0;
    }

    return 1;
}