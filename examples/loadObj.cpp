#include "MotArda/Engine.hpp"
#include "MotArda/ObjLoader.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"

//Ref: https://www.glfw.org/docs/3.3/quick.html#quick_example
//-------Triangle example data-----------------
static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 position;\n"
"attribute vec3 normal;\n"
"varying vec3 fragColor;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(position, 1.0);\n"
"    fragColor = normal * 0.5 + 0.5;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 fragColor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(fragColor, 1.0);\n"
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

		//auto maybeObjLoader = ObjLoader::loadObj("C:/Dvd/MotoresGrafica/3PMG_2526_GalansobRialpe/assets/teapot.obj");
		auto maybeObjLoader = ObjLoader::loadObj("../assets/example.obj");

		if(!maybeObjLoader.has_value()) return 1;

		ObjLoader objLoader = maybeObjLoader.value();
		std::vector<Vertex> vertex = objLoader.getVertices();
		const void* vertexBuffer = static_cast<const void*>(vertex.data());


		eng.windowSetErrorCallback(error_callback);
		eng.windowCreateContext();

		//this is default set to 1
		eng.windowSetSwapInterval();

		//opengl functions
		eng.windowOpenglSetup(
			vertexBuffer,
			vertex_shader_text,
			fragment_shader_text,
			"MVP",
			"position",
			"normal",
			sizeof(vertex[0]),
			vertex.size()
		);

		glm::mat4x4 m, p, mvp;
		float ratio = eng.windowGetSizeRatio();

		float movSpeed = 0.01f;
		float xPos = 0;
		float yPos = -1;

		float rotSpeed = 0.01f;
		float rotationAngle = 0.0f;

		float scaSpeed = 0.01f;
		float scale = 0.25;

		while (!eng.windowShouldClose()) {

			eng.windowInitFrame();


			if (eng.inputIsKeyPressed(Input::Keyboard::D)) {
				xPos += movSpeed;
			}
			else if (eng.inputIsKeyPressed(Input::Keyboard::A)) {
				xPos -= movSpeed;
			}

			if (eng.inputIsKeyPressed(Input::Keyboard::S)) {
				yPos -= movSpeed;
			}
			else if (eng.inputIsKeyPressed(Input::Keyboard::W)) {
				yPos += movSpeed;
			}

			if (eng.inputIsKeyPressed(Input::Keyboard::Q)) {
				rotationAngle -= rotSpeed;
			}
			else if (eng.inputIsKeyPressed(Input::Keyboard::E)) {
				rotationAngle += rotSpeed;
			}

			if (eng.inputIsKeyPressed(Input::Keyboard::Z)) {
				scale -= scaSpeed;
			}
			else if (eng.inputIsKeyPressed(Input::Keyboard::X)) {
				scale += scaSpeed;
			}


			if (eng.inputIsKeyDown(Input::Keyboard::F)) {
				rotationAngle -= rotSpeed * 100;
			}else if (eng.inputIsKeyUp(Input::Keyboard::F)) {
				rotationAngle += rotSpeed * 100;
			}

			float a = (float)eng.windowGetTimer();

			m = glm::mat4(1.0f);
			m = glm::scale(m, { scale, scale, scale });
			m = glm::translate(m, { xPos, yPos, 0 });
			m = glm::rotate(m, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			p = glm::ortho(-ratio, ratio, -1.0f, 1.0f, -1.0f, 1.0f);
			mvp = p * m;

			eng.windowOpenglProgramUniformDraw(glm::value_ptr(mvp), vertex.size());

			eng.windowEndFrame();
		}

		return 0;
	}

	return 1;
}