#pragma once

#include <optional>

namespace MADE {
    int main();

    class Window {
    public:
        ~Window();


        //--Dissable copy constructor and enable noving constructors--
        Window(const Window& right) = delete;
        Window& operator=(const Window& right) = delete;

        Window(Window& right) = default;
        Window& operator=(Window& right) = default;

        //------------Functions-----------------------
        static std::optional<Window> createWindow(
            int width,
            int height,
            const char* windowName);

        bool shouldWindowClose();

        void pollEvents();

    private:
        struct Data;
        Data* data;

        Window(Data* data);
    };
}