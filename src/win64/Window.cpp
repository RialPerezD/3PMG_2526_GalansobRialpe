
#include "Motarda/win64/Window.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <filesystem>

#include <algorithm>
#include <MotArda/common/Engine.hpp>
#include <MotArda/win64/Debug.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h" 

namespace MTRD {
    Window::~Window() {
        if (glfw_window) {
            glfwDestroyWindow(glfw_window);

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        if (glfw_secondary_window) {
            glfwDestroyWindow(glfw_secondary_window);
        }

    }


    Window::Window(Window&& right)
        :glfw_window(right.glfw_window),
        glfw_secondary_window(right.glfw_secondary_window),
        windowWidth_(right.windowWidth_),
        windowHeight_(right.windowHeight_),
        debug_(right.debug_),
        lastFrameTime_(right.lastFrameTime_)
    {
        right.glfw_window = nullptr;
        right.glfw_secondary_window = nullptr;
    }


    void Window::checkErrors() {
        if (debug_) {
            glCheckError();
        }
    }


    Window::Window(GLFWwindow* glfwWindow, GLFWwindow* glfwSecondaryWindow, bool debug) :
        glfw_window(glfwWindow),
        glfw_secondary_window(glfwSecondaryWindow),
        debug_(debug)
    {
        glfwMakeContextCurrent(glfw_window);
        gladLoadGL();


        // Imgui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
		ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		ImGui_ImplOpenGL3_Init("#version 460");

        if (debug_){
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }


    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName, bool debug) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        GLFWwindow* glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);

        if (glfw_window == nullptr) {
            return std::nullopt;
        }

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow* glfw_secondary_window = glfwCreateWindow(1, 1, "", nullptr, glfw_window);

        std::optional<Window> wind = std::make_optional(Window{ glfw_window, glfw_secondary_window, debug });
        wind.value().windowWidth_ = width;
        wind.value().windowHeight_ = height;
        wind.value().debug_ = debug;

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (debug_) {
            glCheckError();
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

                if (debug_) {
                    glCheckError();
                }

                // Configuracion basica de la textura
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                if (debug_) {
                    glCheckError();
                }

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

                if (debug_) {
                    glCheckError();
                }

                stbi_image_free(data);

                if (debug_) {
                    glCheckError();
                }

                textureCache[key] = tex; // Guardamos la textura en la cache
                mat.diffuseTexID = tex;
                continue;
            }
        }
    }
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
