#pragma once

#include <optional>
#include <memory>
#include <vector>
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

        //------------Structs-------------------------
        struct VertexAttrib {
            const char* name;
            int size;
            size_t offset;
        };

        //------------Functions-----------------------
        bool shouldClose();
        void setDebugMode(bool b);
        void pollEvents();
        void createContext();
        double timer();
        void swapBuffers();
        void setSwapInterval(int i);
        void setErrorCallback(void(*function)(int, const char*));
        float getSizeRatio();
        void setKeyCallback(void* keyCallback);

        //------OpenGl functions-----------------------
        void openglGenerateBuffers(const void* vertex, size_t verticeSize, int numVertex);
        void openglGenerateVertexShaders(const char* text);
        void openglGenerateFragmentShaders(const char* text);
        void openglCreateProgram();
        void openglSetAttributesAndUniforms(
            const std::vector<const char*>& uniforms,
            const std::vector<VertexAttrib>& attributes,
            size_t verticeSize
        );

        void openglViewportAndClear();
        void openglProgramUniformDraw(const GLfloat* mvp, int ammountPoints);

    private:
        struct Data;
        std::unique_ptr<Data> data;
        explicit Window(std::unique_ptr<Data> newData); 

        GLuint vertexBuffer, vertexShader, fragmentShader, program, vao;
        std::vector<GLint> uniformLocations;

        int windowWidth_;
        int windowHeight_;

        void checkErrors();
        bool debug_;
    };
}