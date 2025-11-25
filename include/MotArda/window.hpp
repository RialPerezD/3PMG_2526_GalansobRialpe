#pragma once

#include <optional>
#include <memory>
#include <vector>
#include "glad/glad.h"
#include "ObjLoader.hpp"

namespace MTRD {
    int main();

    class Window {
    public:
        struct Data;

        enum class UniformTypes {
            Mat2,
            Mat3,
            Mat4,
            Vec2,
            Vec3,
            Vec4
        };

        static std::optional<Window> windowCreate(
            int width,
            int height,
            const char* windowName);

        ~Window();

        //--Dissable copy constructor and enable noving constructors--
        Window(const Window& right) = delete;
        Window& operator=(const Window& right) = delete;

        Window(Window&& right);
        Window& operator=(Window&& right);

        //------------Structs-------------------------
        struct VertexAttrib {
            const char* name;
            int size;
            size_t offset;
        };

        struct UniformAttrib {
            const char* name;
            GLint location;
            UniformTypes type;
            float* values;
        };

        struct ObjItem {
            std::vector<Shape> shapes;
            std::vector<Material> materials;

            ObjItem();
            ObjItem(const std::vector<Shape>& s, const std::vector<Material>& m)
                : shapes(s), materials(m) {
            }
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
        float getLastFrameTime();

        //------OpenGl functions-----------------------
        void openglGenerateVertexBuffers(const void* vertex, int numVertex, GLuint& vao);
        void openglClearVertexBuffers(GLuint& vao);
        void openglGenerateVertexShaders(const char* text);
        void openglGenerateFragmentShaders(const char* text);
        void openglCreateProgram();
        void openglSetUniformsLocationsAndAtributtes(
            std::vector<Window::UniformAttrib>& uniforms,
            const std::vector<VertexAttrib>& attributes
        );

        void openglViewportAndClear();
        void openglSetUniformsValues(const std::vector<Window::UniformAttrib>& uniforms);
        void openglProgramUniformDraw(Render& render);
        void openglLoadMaterials(std::vector<Material>& materials);


    private:
        std::unique_ptr<Data> data;
        explicit Window(std::unique_ptr<Data> newData); 

        GLuint vertexBuffer, vertexShader, fragmentShader, program;

        int windowWidth_;
        int windowHeight_;

        void checkErrors();
        bool debug_;

        double lastFrameTime_ = 0.0;
    };
}