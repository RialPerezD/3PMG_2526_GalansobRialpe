#pragma once

#include <optional>
#include <memory>
#include <vector>
#include "glad/glad.h"
#include "ObjLoader.hpp"

namespace MTRD {
    int main();
    /**
    * @class Window
    * @brief This class contains the functions to manage the engine window
    */
    class Window {
    public:

        //< Struct that contains the glfw window
        struct Data;

        //< Type of values for the window
        enum class UniformTypes {
            Mat2,
            Mat3,
            Mat4,
            Vec2,
            Vec3,
            Vec4
        };

        /**
        * @brief windowCreate
        * @param int width Horizontal size of the window.
        * @param int height Vertical size of the window.
        * @param const char* windowName Name for the window.
        * @return Returns "wind", the window we have just created.
        * @details First, it creates an unique ptr to store and create
        * the window. Then the window is moved to a new pointer, where
        * width and height are setted.
        */  
        static std::optional<Window> windowCreate(
            int width,
            int height,
            const char* windowName);
        /**
        * @brief ~window
        * @details Destructor of the window class.
        * It calls "glfwDestroyWindow" to release it.
        */
        ~Window();

        /**
        * @brief Copy constructor
        * @param const Window& right Reference to a Window type object
        * @details Copy contructor has been removed.
        */
        Window(const Window& right) = delete;
        /**
        * @brief Copy assignment
        * @param const Window& right Reference to a Window type object
        * @details Copy assignment has been removed.
        */
        Window& operator=(const Window& right) = delete;

        /**
        * @brief Copy Constructor
        * @param const Window& right Reference to a Window type object
        * @details We are able to copy the window as unique_ptr allows it.
        */
        Window(Window&& right);
        /**
        * @brief Copy assignment
        * @param const Window& right Reference to a Window type object
        * @details We are able to copy the window as unique_ptr allows it.
        */
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

        /**
        * @brief shouldClose
        * @return Returns the value of the close flag
        * @details Checks the close flag of the specified window.
        */
        bool shouldClose();
        /**
        * @brief setDebugMode
        * @param bool b
        * @details Sets "debug_" to the specified boolean value
        */
        void setDebugMode(bool b);
        /**
        * @brief pollEvents
        * @details Calls glfwPollEvents, that waits until events 
        * are queued and processes them
        */
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