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