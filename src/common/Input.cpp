#include "Motarda/common/Input.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


namespace MTRD {
	std::vector<int> Input::pressKey = {};
	std::vector<int> Input::repeatKey = {};
	std::vector<int> Input::releaseKey = {};

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		if (action == 1) {
			Input::repeatKey.push_back(key);

			Input::pressKey.push_back(key);
		}
		else if(action == 0){
			Input::repeatKey.erase(std::remove(
				Input::repeatKey.begin(),
				Input::repeatKey.end(),
				key),
			Input::repeatKey.end());

			Input::releaseKey.push_back(key);
		}
	}


	Input Input::inputCreate(Window& window) {
		Input inp;

		inp.setKeyboardCallback(window);

		return inp;
	}

	void Input::clearBuffers() {
		pressKey.clear();
		releaseKey.clear();
	}


	Input::Input() {
		repeatKey.clear();

		clearBuffers();
	}


	void Input::setKeyboardCallback(Window& window) {
		window.setKeyCallback(keyCallback);
	}


	bool checkVector(int numbr, std::vector<int> vector) {
		for (int key : vector) {
			if (numbr == key) {
				return true;
			}
		}

		return false;
	}


	bool Input::isKeyPressed(Keyboard key) {

		if (repeatKey.size() == 0) return false;

		for (int numbr : asciiMap.find(key)->second) {
			if(checkVector(numbr, repeatKey)){
				return true;
			}
		}

		return false;
	}


	bool Input::isKeyDown(Keyboard key) {

		if (pressKey.size() == 0) return false;

		for (int numbr : asciiMap.find(key)->second) {
			if (checkVector(numbr, pressKey)) {
				Input::pressKey.erase(std::remove(
					Input::pressKey.begin(),
					Input::pressKey.end(),
					numbr),
					Input::pressKey.end());

				return true;
			}
		}

		return false;
	}


	bool Input::isKeyUp(Keyboard key) {

		if (releaseKey.size() == 0) return false;

		for (int numbr : asciiMap.find(key)->second) {
			if (checkVector(numbr, releaseKey)) {
				Input::releaseKey.erase(std::remove(
					Input::releaseKey.begin(),
					Input::releaseKey.end(),
					numbr),
					Input::releaseKey.end());

				return true;
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
		repeatKey.clear();
		asciiMap.clear();
	}


	Input::Input(Input&& right) noexcept :
		asciiMap{ right.asciiMap }
	{
	}
}