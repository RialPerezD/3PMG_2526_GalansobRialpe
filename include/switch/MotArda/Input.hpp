#pragma once

#include <MotArda/window.hpp>

#include <optional>
#include <vector>
#include <map>

#include <switch.h>

namespace MTRD {

    class Input {
    public:
        enum class MouseButton {
            Left,
            Right,
            Middle
        };

        enum class Keyboard {
            A, B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            SPACE, ENTER, UP, DOWN, LEFT, RIGHT, ESCAPE,
            SHIFT, CONTROL, ALT, TAB
        };

        struct StickPosition {
            float x = 0;
            float y = 0;
        };

        static Input inputCreate(Window& window);

        ~Input();

        Input(const Input& right) = delete;
        Input& operator=(const Input& right) = delete;

        Input(Input&& right) noexcept;
        Input& operator=(Input&& right) = default;

        bool isKeyPressed(Keyboard);
        bool isKeyDown(Keyboard);
        bool isKeyUp(Keyboard);

        void generateAsciiMap();
        void setKeyboardCallback(Window& window);
        void setMouseButtonCallback(Window& window);
        void getMousePosition(int& x, int& y);
        bool isMouseButtonPressed(MouseButton button);
        bool isMouseButtonDown(MouseButton button);
        void setWindow(Window* window);
        void clearMouseBuffers();
        void clearBuffers();

        static StickPosition getLeftStickPosition();
        static StickPosition getRightStickPosition();
        static bool isTouching();
        static void getTouchPosition(int& x, int& y);
        static int getTouchCount();

        static void poll();

        static std::vector<int> pressKey;
        static std::vector<int> repeatKey;
        static std::vector<int> releaseKey;
        static std::vector<int> pressMouseButton;
        static std::vector<int> releaseMouseButton;
        static std::vector<int> repeatMouseButton;

    private:
        Input();
        std::map<Keyboard, std::vector<int>> asciiMap;
        Window* window_ = nullptr;

        static PadState s_padState;
        static u64 s_prevButtons;
        static bool s_padInitialized;

        static float s_leftStickX;
        static float s_leftStickY;
        static float s_rightStickX;
        static float s_rightStickY;
        static int s_touchX;
        static int s_touchY;
        static int s_touchCount;
    };
}
