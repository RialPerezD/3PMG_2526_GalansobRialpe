#pragma once

#include <optional>
#include "Window.hpp"
#include "Input.hpp"
#include "JobSystem.hpp"

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
			const void* vertexBuffer,
			const char* vertexShader,
			const char* fragmentShader,
			std::vector<Window::UniformAttrib>& uniforms,
			const std::vector<Window::VertexAttrib>& attributes,
			size_t verticeSize,
			int numVertex
		);
		void windowInitFrame();
		void windowOpenglSetUniformsValues(std::vector<Window::UniformAttrib> uniforms);
		void windowOpenglProgramUniformDraw(int ammountPoints);
		void windowSetDebugMode(bool b);
		void close();

		bool inputIsKeyPressed(Input::Keyboard key);
		bool inputIsKeyDown(Input::Keyboard key);
		bool inputIsKeyUp(Input::Keyboard key);

		static std::optional<MotardaEng> createEngine(
			int width = 800,
			int height = 600,
			const char* windowName = "Motarda default name");

	private:
		MotardaEng(Window window, Input input, JobSystem js);
		Window window_;
		Input input_;
		JobSystem jobSystem_;
	};
}