#pragma once

#include <optional>

namespace MTRD {
    int main();

    class Window {
    public:
        ~Window();


        //--Dissable copy constructor and enable noving constructors--
        Window(const Window& right) = delete;
        Window& operator=(const Window& right) = delete;

        Window(Window&& right) = default;
        Window& operator=(Window&& right) = default;

        //------------Functions-----------------------
        static std::optional<Window> windowCreate(
            int width,
            int height,
            const char* windowName);

        bool windowShouldClose();

        void windowPollEvents();

    private:
        struct Data;
        Data* data;

        Window(Data* data);
    };
}