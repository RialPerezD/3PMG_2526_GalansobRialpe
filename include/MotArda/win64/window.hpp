#pragma once

#include <optional>
#include <memory>
#include <vector>
#include "glad/glad.h"
#include "../common/ObjLoader.hpp"


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#pragma warning(pop)

namespace MTRD {
    class Mesh;
    class Material;

    int main();
    /**
    * @class Window
    * @brief This class contains the functions to manage the engine window
    */
    class Window {
    public:
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
        * @brief Move Constructor
        * @param const Window&& right Reference to a Window type object
        * @details We are able to Move the window as unique_ptr allows it.
        */
        Window(Window&& right);
        /**
        * @brief Move assignment
        * @param const Window&& right Reference to a Window type object
        * @details We are able to Move the window as unique_ptr allows it.
        */
        Window& operator=(Window&& right);

        //------------Structs-------------------------
        struct VertexAttrib {
            //< Name of the vertex attribute
            const char* name;
            //< Size of the vertex attribute
            int size;
            //< Offset of the vertex attribute
            size_t offset;
        };

        struct UniformAttrib {
            //< Name of the uniform attribute 
            const char* name;
            //< Location of the uniform attribute
            GLint location;
            //< Type of the uniform attribute
            UniformTypes type;
            //< Values of the uniform attribute
            float* values;
        };

        struct ObjItem {
            //< Vector that contains the objects to draw.
            std::vector<Mesh> meshes;
            //< Vector of the diffetent materials for the shapes.
            std::vector<Material> materials;

            ObjItem();
            ObjItem(const std::vector<Mesh>& s, const std::vector<Material>& m)
                : meshes(s), materials(m) {
            }
        };

        //------------Functions-----------------------

        GLFWwindow* getGlfwWindow() const { return glfw_window; }
        GLFWwindow* getGlfwSecondaryWindow() const { return glfw_secondary_window; }

        /**
        * @brief shouldClose
        * @return Returns the value of the close flag
        * @details Checks the close flag of the specified window.
        */
        bool shouldClose();
        /**
        * @brief pollEvents
        * @details Calls glfwPollEvents, that waits until events 
        * are queued and processes them.
        */
        void pollEvents();
        /**
        * @brief timer
        * @return Returns the GLFW time.
        * @details Return the current GLFW time in seconds by calling "glfwGetTime".
        */
        double timer();
        /**
        * @brief swapBuffers
        * @details Calls glfwSwapBuffers in order to swap thre front and
        * back buffers of the specified window.
        */
        void swapBuffers();
        /**
        * @brief setErrorCallback
        * @param void(*function)(int, const char*) Function to callback
        * @details Calls glfwSetErrorCallback in order to set the 
        * error callback.
        */
        void setErrorCallback(void(*function)(int, const char*));
        /**
        * @brief getSizeRatio.
        * @return Returns the screen ratio.
        * @details Returns the scree ratio by dividing windowWidth_ by 
        * windowHeight_.
        */
        float getSizeRatio();
        /**
        * @brief setKeyCallback
        * @param void* keyCallback
        * @details Calls glfwSetKeyCallback in order to set the
        * key callback of the specified window, which is called
        * when a key is pressed, repeated or released.
        */
        void setKeyCallback(void* keyCallback);
        /**
        * @brief getLastFrameTime.
        * @return Returns delta time, a substraction between the result
        * of timer() before and after the operation.
        * @details Calls timer() to manage the time in seconds.
        */
        float getLastFrameTime();

        //------OpenGl functions-----------------------
        /**
        * @brief openglSetUniformsLocationsAndAtributtes.
        * @param std::vector<Window::UniformAttrib>& uniforms Vector of
        * uniforms.
        * @param const std::vector<VertexAttrib>& attributes Vector of 
        * attributes
        * @details This function sets up the connection between the 
        * data and the OpenGL shader.
        */
        /*void openglSetUniformsLocationsAndAtributtes(
            std::vector<Window::UniformAttrib>& uniforms,
            const std::vector<VertexAttrib>& attributes
        );*/

        /**
        * @brief openglViewportAndClear.
        * @details Clears the buffer of the window
        */
        void openglViewportAndClear();
        /**
        * @brief openglSetUniformsValues.
        * @param const std::vector<Window::UniformAttrib>& uniforms Uniforms
        * of the window.
        * @details This function sets all the uniforms of the window.
        */
        void openglSetUniformsValues(const std::vector<Window::UniformAttrib>& uniforms);
        /**
        * @brief openglLoadMaterials.
        * @param std::vector<Material>& materials Material for a render.
        * @details First it generates and binds the texture. Then, the basic
        * configuration is setted, adn finally, the different channels are read
        * and the texture is generated.
        */
        void openglLoadMaterials(std::vector<Material>& materials);

        
    private:

        /**
        * @brief Constructor
        * @param std::unique_ptr<Data> newData Unique ptr of the Data struct.
        * @details Inits the Window class data.
        */
        explicit Window(GLFWwindow* glfwWindow, GLFWwindow* glfwSecondaryWindow, bool debug);

        void checkErrors();

        // Main Window
        GLFWwindow* glfw_window;
        // Secondary Window
        GLFWwindow* glfw_secondary_window;

        //< Horizontal size of the window
        int windowWidth_;
        //< Vertical size of the window
        int windowHeight_;
        //< Boolean that activates the debug mode
        bool debug_;
        //< Last frame time
        double lastFrameTime_ = 0.0;
    };
}