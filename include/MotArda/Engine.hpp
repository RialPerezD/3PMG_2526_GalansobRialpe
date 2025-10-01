#pragma once

#include <optional>
#include "Window.hpp"

namespace MADE {

	class MotardaEng {

	public:
		~MotardaEng();

		MotardaEng(const MotardaEng& right) = delete;
		MotardaEng& operator=(const MotardaEng& right) = delete;

		MotardaEng(MotardaEng& right);
		MotardaEng& operator=(MotardaEng& right);

		bool shouldWindowClose();
		void endWindowFrame();
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