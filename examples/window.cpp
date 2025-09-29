#include "MotArda/Window.hpp"

#include <memory>

int main() {

	// Create a blank window
	auto window = std::make_unique<Window>(800, 600, "Motarda window");

	// Check if not null
	if (!window) {
		return 1;
	}

	// Main loop, here refresh or update all
	while (!window->shouldWindowClose()) {
		window->pollEvents();
	}
}