#include "MotArda/Engine.hpp"

#include <memory>

int MTRD::main() {

	// Create a blank window
	auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda window");

	// Check if not null
	if (maybeEng) {
		/*MTRD::MotardaEng eng = maybeEng.value();

		// Main loop, here refresh or update all
		while (!eng.windowShouldClose()) {
			eng.windowEndFrame();
		}*/

		return 0;
	}

	return 1;
}