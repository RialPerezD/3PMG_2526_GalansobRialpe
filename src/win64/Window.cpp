
#include "Motarda/win64/Window.hpp"
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h" 


#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4005)
//Need this include to use WinMain
#include <windows.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#pragma warning(pop)


#include <algorithm>
#include <MotArda/common/Engine.hpp>
#include <MotArda/win64/Debug.hpp>


namespace MTRD {

    struct Window::Data {
        GLFWwindow* glfw_window;
        //< Horizontal size of the window
        int windowWidth_;
        //< Vertical size of the window
        int windowHeight_;
        //< Boolean that activates the debug mode
        bool debug_;
        //< Last frame time
        double lastFrameTime_ = 0.0;
    };


    Window::~Window() {
        if (data) {
            if (data->glfw_window) {
                glfwDestroyWindow(data->glfw_window);
            }
        }
    }


    // Must be defined here as Data is not defined in the header
    Window::Window(Window&& right) = default;
    Window& Window::operator=(Window&& right) = default;


    void Window::checkErrors() {
        if (data->debug_) {
            glCheckError();
        }
    }


    Window::Window(std::unique_ptr<Data> newData,bool debug) :
        data(std::move(newData))
    {
        glfwMakeContextCurrent(data->glfw_window);
        gladLoadGL();

        if (data->debug_)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }


    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName) {

        auto d = std::make_unique<Data>();


        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        d->glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);

        if (d->glfw_window == nullptr) {
            return std::nullopt;
        }
        // TODO pasar debug desde parametros de windowCreate
        std::optional<Window> wind = std::make_optional(Window{ std::move(d) ,true });
        wind.value().data->windowWidth_ = width;
        wind.value().data->windowHeight_ = height;
        wind.value().data->debug_ = false;

        glfwSwapInterval(1);

        return wind;
    }


    bool Window::shouldClose() {
        return glfwWindowShouldClose(data->glfw_window);
    }


    void Window::pollEvents() {
        glfwPollEvents();
    }


    double Window::timer() {
        return glfwGetTime();
    }


    void Window::swapBuffers() {
        glfwSwapBuffers(data->glfw_window);
    }


    void Window::setErrorCallback(void(*function)(int, const char*)) {
        glfwSetErrorCallback(function);
    }


    float Window::getSizeRatio() {
        return data->windowWidth_ / (float)data->windowHeight_;
    }


    void Window::setKeyCallback(void* keyCallback) {
        auto parsed = reinterpret_cast<void(*)(GLFWwindow*, int, int, int, int)>(keyCallback);
        glfwSetKeyCallback(data->glfw_window, parsed);
    }


    float Window::getLastFrameTime() {
        double currentTime = timer();
        float deltaTime = static_cast<float>(currentTime - data->lastFrameTime_);
        data->lastFrameTime_ = currentTime;
        return deltaTime;
    }


    void Window::openglViewportAndClear() {
        glViewport(0, 0, data->windowWidth_, data->windowHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (data->debug_) {
            glCheckError();
        }
    }


    void Window::openglSetUniformsValues(const std::vector<Window::UniformAttrib>& uniforms) {
        for (int i = 0; i < uniforms.size(); i++) {
            float f = *(uniforms[i].values);
            switch (uniforms[i].type) {
            case Window::UniformTypes::Vec2:
                glUniform2fv(uniforms[i].location, 1, uniforms[i].values);
                break;
            case Window::UniformTypes::Vec3:
                glUniform3fv(uniforms[i].location, 1, uniforms[i].values);
                break;
            case Window::UniformTypes::Vec4:
                glUniform4fv(uniforms[i].location, 1, uniforms[i].values);
                break;
            case Window::UniformTypes::Mat2:
                glUniformMatrix2fv(uniforms[i].location, 1, GL_FALSE, uniforms[i].values);
                break;
            case Window::UniformTypes::Mat3:
                glUniformMatrix3fv(uniforms[i].location, 1, GL_FALSE, uniforms[i].values);
                break;
            case Window::UniformTypes::Mat4:
                glUniformMatrix4fv(uniforms[i].location, 1, GL_FALSE, uniforms[i].values);
                break;
            default:
                break;
            }

            if (data->debug_) {
                glCheckError();
            }
        }
    }


    void Window::openglLoadMaterials(std::vector<Material>& materials) {
        std::unordered_map<std::string, GLuint> textureCache;

        for (auto& mat : materials) {
            if (!mat.diffuseTexPath.empty()) {
                
                std::string route = mat.diffuseTexPath;

                std::string key(route);
                if (textureCache.find(key) != textureCache.end()) {
                    mat.diffuseTexID = textureCache[key];
                    continue;
                }

                GLuint tex = -1;
                glGenTextures(1, &tex);
                glBindTexture(GL_TEXTURE_2D, tex);

                // Configuracion basica de la textura
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                int width, height, channels;
                stbi_set_flip_vertically_on_load(true); // Invierte verticalmente para OpenGL
                unsigned char* data = stbi_load(route.c_str(), &width, &height, &channels, 0);

                if (!data) {
                    std::cerr << "Error cargando textura: " << route << std::endl;
                    mat.diffuseTexID = -1; // -1 significa sin textura
                    continue;
                }

                GLenum format;

                switch (channels) {
                case 1:
                    format = GL_RED;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    format = GL_RGB;
                    break;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);

                textureCache[key] = tex; // Guardamos la textura en la cache
                mat.diffuseTexID = tex;
                continue;
            }
        }
    }

    Window::ObjItem::ObjItem() : meshes(), materials() {}
}


// Entry point for window:
int main()
{
    // Initializes the GLFW library and prepares it for use.
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwInit();

    // Calls the main implemented by the usser
    MTRD::main();

    // Ileans up and releases all resources used by the GLFW library.
    glfwTerminate();

    return 0;
}
