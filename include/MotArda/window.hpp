#pragma once

#include <optional>

namespace MTRD {
    int main();

    class Window {
    public:
        static std::optional<Window> windowCreate(
            int width,
            int height,
            const char* windowName);

        ~Window();

        //--Dissable copy constructor and enable noving constructors--
        Window(const Window& right) = delete;
        Window& operator=(const Window& right) = delete;

        Window(Window&& right);
        Window& operator=(Window&& right) = default;

        //------------Functions-----------------------

        bool windowShouldClose();
        void windowPollEvents();
        void windowCreateContext();
        void windowRender();
        double windowTimer();
        void windowSwapBuffers();
        void windowSetSwapInterval(int i);

    private:
        struct Data;
        Data* data;

        int windowWidth_;
        int windowHeight_;

        Window(Data* data);
    };
}