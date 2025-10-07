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

    private:
        struct Data;
        Data* data;

        Window(Data* data);
    };
}