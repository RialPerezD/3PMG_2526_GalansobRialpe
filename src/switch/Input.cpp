#include <MotArda/Input.hpp>

#include <algorithm>

namespace MTRD {
    std::vector<int> Input::pressKey = {};
    std::vector<int> Input::repeatKey = {};
    std::vector<int> Input::releaseKey = {};
    std::vector<int> Input::pressMouseButton = {};
    std::vector<int> Input::releaseMouseButton = {};
    std::vector<int> Input::repeatMouseButton = {};

    PadState Input::s_padState;
    u64 Input::s_prevButtons = 0;
    bool Input::s_padInitialized = false;

    Input::Input() {
    }

    Input Input::inputCreate(Window& window) {
        Input inp;
        if (!s_padInitialized) {
            padConfigureInput(1, HidNpadStyleSet_NpadStandard);
            padInitializeDefault(&s_padState);
            s_padInitialized = true;
        }
        (void)window;
        return inp;
    }

    void Input::poll() {
        if (!s_padInitialized) return;

        padUpdate(&s_padState);
        u64 cur = padGetButtons(&s_padState);
        u64 prev = s_prevButtons;

        repeatKey.clear();
        for (int i = 0; i < 32; i++) {
            if (cur & (1ULL << i)) {
                repeatKey.push_back(i);
            }
        }

        pressKey.clear();
        u64 down = cur & ~prev;
        for (int i = 0; i < 32; i++) {
            if (down & (1ULL << i)) {
                pressKey.push_back(i);
            }
        }

        releaseKey.clear();
        u64 up = prev & ~cur;
        for (int i = 0; i < 32; i++) {
            if (up & (1ULL << i)) {
                releaseKey.push_back(i);
            }
        }

        s_prevButtons = cur;
    }

    void Input::clearBuffers() {
        pressKey.clear();
        releaseKey.clear();
    }

    void Input::clearMouseBuffers() {
        pressMouseButton.clear();
        releaseMouseButton.clear();
    }

    void Input::setKeyboardCallback(Window& window) {
        (void)window;
    }

    void Input::setMouseButtonCallback(Window& window) {
        (void)window;
    }

    void Input::setWindow(Window* window) {
        window_ = window;
    }

    void Input::getMousePosition(int& x, int& y) {
        x = 0;
        y = 0;
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
        // Map Switch HID buttons (bit index) to Keyboard enum
        // HidNpadButton bits:
        //   0: A, 1: B, 2: X, 3: Y
        //   4: LStick, 5: RStick, 6: L, 7: R
        //   8: ZL, 9: ZR, 10: Plus, 11: Minus
        //   12: Left, 13: Up, 14: Right, 15: Down
        //   16: LeftSL, 17: LeftSR, 18: RightSL, 19: RightSR

        // D-pad -> WASD
        asciiMap[Keyboard::W].push_back(13);  // Up -> W
        asciiMap[Keyboard::S].push_back(15);  // Down -> S
        asciiMap[Keyboard::A].push_back(12);  // Left -> A
        asciiMap[Keyboard::D].push_back(14);  // Right -> D

        // Face buttons
        asciiMap[Keyboard::E].push_back(0);   // A -> E (move up)
        asciiMap[Keyboard::Q].push_back(1);   // B -> Q (move down)
        asciiMap[Keyboard::R].push_back(2);   // X -> R (rotate)
        asciiMap[Keyboard::T].push_back(3);   // Y -> T (rotate)

        // System buttons
        asciiMap[Keyboard::ESCAPE].push_back(10);  // Plus -> ESCAPE
        asciiMap[Keyboard::SPACE].push_back(11);   // Minus -> SPACE

        // Shoulder buttons
        asciiMap[Keyboard::SHIFT].push_back(6);   // L -> SHIFT
        asciiMap[Keyboard::CONTROL].push_back(7); // R -> CONTROL
        asciiMap[Keyboard::ALT].push_back(8);     // ZL -> ALT
        asciiMap[Keyboard::ENTER].push_back(9);   // ZR -> ENTER

        // Left stick click -> TAB
        asciiMap[Keyboard::TAB].push_back(4);     // LStick -> TAB
        // Right stick click -> (unused, keep as spare)
    }

    bool Input::isMouseButtonPressed(MouseButton button) {
        (void)button;
        return false;
    }

    bool Input::isMouseButtonDown(MouseButton button) {
        (void)button;
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
