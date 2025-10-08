#pragma once

#include <optional>
#include "Window.hpp"

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
			const char* at1,
			const char* at2,
			const char* at3,
			double verticeSize
		);
		void windowOpenglViewportAndClear();
		void windowOpenglProgramUniformDraw(const GLfloat* mvp);
		void close();


		static std::optional<MotardaEng> createEngine(
			int width = 800,
			int height = 600,
			const char* windowName = "Motarda default name");

	private:
		MotardaEng(Window& window);
		Window window_;
	};
}