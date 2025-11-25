#include "MotArda/Engine.hpp"

#include <memory>


int MTRD::main() {

	// Create a blank window
	auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda window");

	// Check if not null
    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        while (!eng.windowShouldClose()) {
            eng.windowEndFrame();
        }

        return 0;
    }

    return 1;
}