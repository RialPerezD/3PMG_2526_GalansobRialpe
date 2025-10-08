#pragma once

#include <optional>
#include <GL/glew.h>

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

        bool shouldClose();
        void pollEvents();
        void createContext();
        void render();
        double timer();
        void swapBuffers();
        void setSwapInterval(int i);
        void setErrorCallback(void(*function)(int, const char*));
        float getSizeRatio();

        //------OpenGl functions-----------------------
        void openglGenerateBuffers(const void* vertices);
        void openglGenerateVertexShaders(const char* text);
        void openglGenerateFragmentShaders(const char* text);
        void openglCreateProgram();
        void openglSet3AtribLocations(const char* at1, const char* at2, const char* at3);
        void openglVertexConfig(double size);
        void openglViewportAndClear();
        void openglProgramUniformDraw(const GLfloat* mvp);

    private:
        struct Data;
        Data* data;

        GLuint vertexBuffer, vertexShader, fragmentShader, program;
        GLint mvpLocation, vposLocation, vcolLocation;

        int windowWidth_;
        int windowHeight_;

        Window(Data* data);
    };
}