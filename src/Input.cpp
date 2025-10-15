#include "Motarda/Input.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


namespace MTRD {
	std::vector<int> Input::pressedKey = {};

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		if (action == 1) {
			Input::pressedKey.push_back(key);
		}
		else if(action == 0){
			Input::pressedKey.erase(std::remove(
				Input::pressedKey.begin(),
				Input::pressedKey.end(),
				key),
			Input::pressedKey.end());
		}
	}


	Input Input::inputCreate(Window& window) {
		Input inp;

		inp.setKeyboardCallback(window);

		return inp;
	}


	Input::Input() {
		pressedKey.clear();
	}


	void Input::setKeyboardCallback(Window& window) {
		window.setKeyCallback(keyCallback);
	}


	bool Input::isKeyPressed(Keyboard key) {

		for (int numbr : asciiMap.find(key)->second) {
			for (int key : pressedKey) {
				if (numbr == key) {
					return true;
				}
			}
		}

		return false;
	}


	void Input::generateAsciiMap() {
		for (int i = 0; i < 27; i++) {
			asciiMap[static_cast<Keyboard>(i)].push_back(65 + i);
			asciiMap[static_cast<Keyboard>(i)].push_back(97 + i);
		}
	}


	Input::~Input() {
		pressedKey.clear();
		asciiMap.clear();
	}


	Input::Input(Input&& right) : 
		asciiMap {right.asciiMap}
	{
	}
}