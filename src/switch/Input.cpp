#include <MotArda/Input.hpp>
#include <fstream>

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

    float Input::s_leftStickX = 0.0f;
    float Input::s_leftStickY = 0.0f;
    float Input::s_rightStickX = 0.0f;
    float Input::s_rightStickY = 0.0f;
    int Input::s_touchX = 0;
    int Input::s_touchY = 0;
    int Input::s_touchCount = 0;

    Input::Input() {
    }

    Input Input::inputCreate(Window& window) {
        Input inp;
        if (!s_padInitialized) {
            padConfigureInput(8, HidNpadStyleSet_NpadStandard);
            padInitializeAny(&s_padState);
            hidInitializeTouchScreen();
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
        pressKey.clear();
        releaseKey.clear();

        u64 down = cur & ~prev;
        u64 up = prev & ~cur;

        for (int i = 0; i < 20; i++) {
            u64 mask = 1ULL << i;
            if (cur & mask)  repeatKey.push_back(i);
            if (down & mask) pressKey.push_back(i);
            if (up & mask)   releaseKey.push_back(i);
        }

        s_prevButtons = cur;

        HidAnalogStickState leftStick = padGetStickPos(&s_padState, 0);
        HidAnalogStickState rightStick = padGetStickPos(&s_padState, 1);
        s_leftStickX = (float)leftStick.x / 32767.0f;
        s_leftStickY = (float)leftStick.y / 32767.0f;
        s_rightStickX = (float)rightStick.x / 32767.0f;
        s_rightStickY = (float)rightStick.y / 32767.0f;

        HidTouchScreenState touchState = { 0 };
        if (hidGetTouchScreenStates(&touchState, 1) == 0) {
            s_touchCount = touchState.count;
            if (s_touchCount > 0) {
                s_touchX = touchState.touches[0].x;
                s_touchY = touchState.touches[0].y;
            }
        }
        else {
            s_touchCount = 0;
        }
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

    static bool checkVector(int numbr, const std::vector<int>& vector) {
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
        // Map Switch HID buttons (bit position) to Keyboard enum
        // HidNpadButton bit positions:
        //   0: A,        1: B,        2: X,        3: Y
        //   4: StickL,   5: StickR,   6: L,        7: R
        //   8: ZL,       9: ZR,      10: Plus,    11: Minus
        //  12: Left,    13: Up,      14: Right,   15: Down
        //  16: LeftSL,  17: LeftSR,  18: RightSL, 19: RightSR

        // D-pad -> WASD
        asciiMap[Keyboard::W].push_back(13);
        asciiMap[Keyboard::S].push_back(15);
        asciiMap[Keyboard::A].push_back(12);
        asciiMap[Keyboard::D].push_back(14);

        // Face buttons
        asciiMap[Keyboard::E].push_back(0);
        asciiMap[Keyboard::Q].push_back(1);
        asciiMap[Keyboard::R].push_back(2);
        asciiMap[Keyboard::T].push_back(3);

        // System buttons
        asciiMap[Keyboard::ESCAPE].push_back(10);
        asciiMap[Keyboard::SPACE].push_back(11);

        // Shoulder buttons
        asciiMap[Keyboard::SHIFT].push_back(6);
        asciiMap[Keyboard::CONTROL].push_back(7);
        asciiMap[Keyboard::ALT].push_back(8);
        asciiMap[Keyboard::ENTER].push_back(9);

        // Stick clicks
        asciiMap[Keyboard::TAB].push_back(4);
        asciiMap[Keyboard::TAB].push_back(5);
    }

    bool Input::isMouseButtonPressed(MouseButton button) {
        (void)button;
        return false;
    }

    bool Input::isMouseButtonDown(MouseButton button) {
        (void)button;
        return false;
    }

    Input::StickPosition Input::getLeftStickPosition() {
        return { s_leftStickX, s_leftStickY };
    }

    Input::StickPosition Input::getRightStickPosition() {
        return { s_rightStickX, s_rightStickY };
    }

    bool Input::isTouching() {
        return s_touchCount > 0;
    }

    void Input::getTouchPosition(int& x, int& y) {
        x = s_touchX;
        y = s_touchY;
    }

    int Input::getTouchCount() {
        return s_touchCount;
    }

    Input::~Input() {
    }

    Input::Input(Input&& right) noexcept :
        asciiMap{ std::move(right.asciiMap) },
        window_{ right.window_ } {
        right.window_ = nullptr;
    }
}
