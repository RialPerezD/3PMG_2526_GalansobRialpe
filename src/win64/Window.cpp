
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


#include <algorithm>
#include <MotArda/common/Engine.hpp>
#include <MotArda/win64/Debug.hpp>


namespace MTRD {
    Window::~Window() {
        if (glfw_window) {
            glfwDestroyWindow(glfw_window);
        }

        if (glfw_secondary_window) {
            glfwDestroyWindow(glfw_secondary_window);
        }
    }


    // Must be defined here as Data is not defined in the header
    Window::Window(Window&& right) {
        this->glfw_window = right.glfw_window;
        this->glfw_secondary_window = right.glfw_secondary_window;
        this->windowWidth_ = right.windowWidth_;
        this->windowHeight_ = right.windowHeight_;

        right.glfw_window = nullptr;
        right.glfw_secondary_window = nullptr;
    };
    Window& Window::operator=(Window&& right) = default;


    void Window::checkErrors() {
        if (debug_) {
            glCheckError();
        }
    }


    Window::Window(GLFWwindow* glfwWindow, GLFWwindow* glfwSecondaryWindow, bool debug) :
        glfw_window(glfwWindow), glfw_secondary_window(glfwSecondaryWindow)
    {
        glfwMakeContextCurrent(glfw_window);
        gladLoadGL();

        if (debug_)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }


    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        GLFWwindow* glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);

        if (glfw_window == nullptr) {
            return std::nullopt;
        }

        GLFWwindow* glfw_secondary_window = glfwCreateWindow(1, 1, "", nullptr, glfw_window);

        // TODO pasar debug desde parametros de windowCreate
        std::optional<Window> wind = std::make_optional(Window{ glfw_window, glfw_secondary_window, true });
        wind.value().windowWidth_ = width;
        wind.value().windowHeight_ = height;
        wind.value().debug_ = false;

        glfwSwapInterval(1);

        return wind;
    }


    bool Window::shouldClose() {
        return glfwWindowShouldClose(glfw_window);
    }


    void Window::pollEvents() {
        glfwPollEvents();
    }


    double Window::timer() {
        return glfwGetTime();
    }


    void Window::swapBuffers() {
        glfwSwapBuffers(glfw_window);
    }


    void Window::setErrorCallback(void(*function)(int, const char*)) {
        glfwSetErrorCallback(function);
    }


    float Window::getSizeRatio() {
        return windowWidth_ / (float)windowHeight_;
    }


    void Window::setKeyCallback(void* keyCallback) {
        auto parsed = reinterpret_cast<void(*)(GLFWwindow*, int, int, int, int)>(keyCallback);
        glfwSetKeyCallback(glfw_window, parsed);
    }


    float Window::getLastFrameTime() {
        double currentTime = timer();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime_);
        lastFrameTime_ = currentTime;
        return deltaTime;
    }


    void Window::openglViewportAndClear() {
        glViewport(0, 0, windowWidth_, windowHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (debug_) {
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

            if (debug_) {
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
