#pragma once

#include <optional>
#include "Window.hpp"
#include "Input.hpp"
#include "JobSystem.hpp"
#include "ObjLoader.hpp"

namespace MTRD {

	class MotardaEng {

	public:
		~MotardaEng() = default;

		MotardaEng(const MotardaEng& right) = delete;
		MotardaEng& operator=(const MotardaEng& right) = delete;

		MotardaEng(MotardaEng&& right) = default;
		MotardaEng& operator=(MotardaEng&& right) = default;

		bool windowShouldClose();
		double windowGetTimer();
		void windowEndFrame();
		void windowCreateContext();
		void windowSetSwapInterval(int i = 1);
		void windowSetErrorCallback(void(*function)(int, const char*));
		float windowGetSizeRatio();
		void windowOpenglSetup(
			std::vector<MTRD::Window::ObjItem> objItemList,
			const char* vertexShader,
			const char* fragmentShader,
			std::vector<Window::UniformAttrib>& uniforms,
			const std::vector<Window::VertexAttrib>& attributes
		);
		void windowInitFrame();
		void windowOpenglSetUniformsValues(std::vector<Window::UniformAttrib> uniforms);
		void windowOpenglProgramUniformDraw(std::vector<MTRD::Window::ObjItem> objItemsList);
		void windowSetDebugMode(bool b);
		void windowLoadAllMaterials(std::vector<MTRD::Window::ObjItem>& objItemsList);
		void close();

		bool inputIsKeyPressed(Input::Keyboard key);
		bool inputIsKeyDown(Input::Keyboard key);
		bool inputIsKeyUp(Input::Keyboard key);

		void enqueueTask(std::function<void()> task);

		std::vector<MTRD::Window::ObjItem> loadObjs(std::vector <const char*> routes);
		const char* loadShaderFile(const char* filename);
		void generateVertexBuffers(std::vector<MTRD::Window::ObjItem> objItemList);

		static std::optional<MotardaEng> createEngine(
			int width = 800,
			int height = 600,
			const char* windowName = "Motarda default name");

	private:
		MotardaEng(Window window, Input input, JobSystem js);
		Window window_;
		Input input_;
		JobSystem jobSystem_;
		
		void windowLoadMaterials(std::vector<Material>& materials);
	};
}