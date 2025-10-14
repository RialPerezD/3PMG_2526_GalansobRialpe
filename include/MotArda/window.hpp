#pragma once

#include <optional>
#include <memory>
#include "glad/glad.h"

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
        void setKeyCallback(void* keyCallback);

        //------OpenGl functions-----------------------
        void openglGenerateBuffers(const void* vertices, size_t verticeSize);
        void openglGenerateVertexShaders(const char* text);
        void openglGenerateFragmentShaders(const char* text);
        void openglCreateProgram();
        void openglSet3AtribLocations(const char* uni1, const char* at1, const char* at2);
        void openglVertexConfig(size_t size);
        void openglViewportAndClear();
        void openglProgramUniformDraw(const GLfloat* mvp);

    private:
        struct Data;
        std::unique_ptr<Data> data;
        explicit Window(std::unique_ptr<Data> newData); 

        GLuint vertexBuffer, vertexShader, fragmentShader, program;
        GLint mvpLocation, vposLocation, vcolLocation;

        int windowWidth_;
        int windowHeight_;


        GLchar log[1000];
        GLsizei log_length;

    };
}