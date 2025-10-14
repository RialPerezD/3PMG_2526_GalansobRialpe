#include "Motarda/Input.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


namespace MTRD {


	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Input::pressedKey.push_back(key);
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


	void Input::clearInputs() {
		pressedKey.clear();
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

	}


	Input::Input(Input&& right) : 
		asciiMap {right.asciiMap}
	{
	}
}