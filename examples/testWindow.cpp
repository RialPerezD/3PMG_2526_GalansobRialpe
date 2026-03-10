#include "MotArda/common/Engine.hpp"

#include <memory>


int MTRD::main() {

    // Init engine with the screen width, height and name
	auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda window");

    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        // The main loop will run until the user closes the window
        while (!eng.windowShouldClose()) {

            // Creates a new frame
            eng.windowInitFrame();
            // Ends the frame
            eng.windowEndFrame();
        }

        return 0;
    }

    return 1;
}