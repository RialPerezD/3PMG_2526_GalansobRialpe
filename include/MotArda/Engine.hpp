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
		void windowEndFrame();
		void windowCreateContext();
		void windowSetSwapInterval(int i = 1);
		void windowSetErrorCallback(void(*function)(int, const char*));
		void windowOpenglGenerateBuffers(const void* vertex);
		void windowOpenglGenerateVertexShaders(const char* text);
		void windowOpenglGenerateFragmentShaders(const char* text);
		void windowOpenglCreateProgram();
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