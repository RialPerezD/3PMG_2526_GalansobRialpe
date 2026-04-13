#include "Motarda/common/Input.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace MTRD {
    std::vector<int> Input::pressKey = {};
    std::vector<int> Input::repeatKey = {};
    std::vector<int> Input::releaseKey = {};
    std::vector<int> Input::pressMouseButton = {};
    std::vector<int> Input::releaseMouseButton = {};
    std::vector<int> Input::repeatMouseButton = {};

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == 1) {
            Input::repeatKey.push_back(key);
            Input::pressKey.push_back(key);
        } else if (action == 0) {
            Input::repeatKey.erase(std::remove(
                Input::repeatKey.begin(),
                Input::repeatKey.end(),
                key),
                Input::repeatKey.end());

            Input::releaseKey.push_back(key);
        }
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (action == 1) {
            Input::repeatMouseButton.push_back(button);
            Input::pressMouseButton.push_back(button);
        } else if (action == 0) {
            Input::repeatMouseButton.erase(std::remove(
                Input::repeatMouseButton.begin(),
                Input::repeatMouseButton.end(),
                button),
                Input::repeatMouseButton.end());
            Input::releaseMouseButton.push_back(button);
        }
    }

    Input Input::inputCreate(Window& window) {
        Input inp;
        inp.setKeyboardCallback(window);
        inp.setMouseButtonCallback(window);
        return inp;
    }

    void Input::clearBuffers() {
        pressKey.clear();
        releaseKey.clear();
    }

    void Input::clearMouseBuffers() {
        pressMouseButton.clear();
        releaseMouseButton.clear();
    }

    Input::Input() {
    }

    void Input::setKeyboardCallback(Window& window) {
        window.setKeyCallback(keyCallback);
    }

    void Input::setMouseButtonCallback(Window& window) {
        window.setMouseButtonCallback(mouseButtonCallback);
    }

    void Input::setWindow(Window* window) {
        window_ = window;
    }

    void Input::getMousePosition(int& x, int& y) {
        if (window_) {
            window_->getMousePosition(x, y);
        }
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
        if (repeatKey.empty()) return false;

        auto it = asciiMap.find(key);
        if (it == asciiMap.end()) return false;

        for (int numbr : it->second) {
            if (checkVector(numbr, repeatKey)) {
                return true;
            }
        }
        return false;
    }

    bool Input::isKeyDown(Keyboard key) {
        if (pressKey.empty()) return false;

        auto it = asciiMap.find(key);
        if (it == asciiMap.end()) return false;

        for (int numbr : it->second) {
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
        if (releaseKey.empty()) return false;

        auto it = asciiMap.find(key);
        if (it == asciiMap.end()) return false;

        for (int numbr : it->second) {
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

    bool Input::isMouseButtonPressed(MouseButton button) {
        int btn = -1;
        switch (button) {
        case MouseButton::Left: btn = GLFW_MOUSE_BUTTON_LEFT; break;
        case MouseButton::Right: btn = GLFW_MOUSE_BUTTON_RIGHT; break;
        case MouseButton::Middle: btn = GLFW_MOUSE_BUTTON_MIDDLE; break;
        }

        return checkVector(btn, repeatMouseButton);
    }

    bool Input::isMouseButtonDown(MouseButton button) {
        int btn = -1;
        switch (button) {
        case MouseButton::Left: btn = GLFW_MOUSE_BUTTON_LEFT; break;
        case MouseButton::Right: btn = GLFW_MOUSE_BUTTON_RIGHT; break;
        case MouseButton::Middle: btn = GLFW_MOUSE_BUTTON_MIDDLE; break;
        }

        if (checkVector(btn, pressMouseButton)) {
            Input::pressMouseButton.erase(std::remove(
                Input::pressMouseButton.begin(),
                Input::pressMouseButton.end(),
                btn),
                Input::pressMouseButton.end());
            return true;
        }
        return false;
    }

    Input::~Input() {
    }

    Input::Input(Input&& right) noexcept :
        asciiMap{ std::move(right.asciiMap) },
        window_{ right.window_ } {
        right.window_ = nullptr;
    }
}