#include <MotArda/Engine.hpp>
#include <MotArda/Debug.hpp>
#include <MotArda/window.hpp>
#include <MotArda/Input.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace MTRD {

    static double GetSystemTimeSeconds() {
        return (double)svcGetSystemTick() / 19200000.0;
    }

    Window::~Window() {
        if (s_display) {
            eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (s_context) {
                eglDestroyContext(s_display, s_context);
                s_context = nullptr;
            }
            if (s_surface) {
                eglDestroySurface(s_display, s_surface);
                s_surface = nullptr;
            }
            eglTerminate(s_display);
            s_display = nullptr;
        }
    }

    Window::Window(Window&& right)
        : s_display(right.s_display),
        s_context(right.s_context),
        s_surface(right.s_surface),
        windowWidth_(right.windowWidth_),
        windowHeight_(right.windowHeight_),
        debug_(right.debug_),
        lastFrameTime_(right.lastFrameTime_)
    {
        right.s_display = nullptr;
        right.s_context = nullptr;
        right.s_surface = nullptr;
    }

    void Window::checkErrors() {
        if (debug_) {
            glCheckError();
        }
    }

    Window::Window(bool debug) :
        s_display(nullptr),
        s_context(nullptr),
        s_surface(nullptr),
        debug_(debug)
    {
    }

    bool Window::initEgl(NWindow* win) {
        s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (!s_display) {
            return false;
        }

        eglInitialize(s_display, nullptr, nullptr);

        if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
            return false;
        }

        EGLConfig config;
        EGLint numConfigs;
        static const EGLint framebufferAttributeList[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_RED_SIZE,     8,
            EGL_GREEN_SIZE,   8,
            EGL_BLUE_SIZE,    8,
            EGL_ALPHA_SIZE,   8,
            EGL_DEPTH_SIZE,   24,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
        };
        eglChooseConfig(s_display, framebufferAttributeList, &config, 1, &numConfigs);
        if (numConfigs == 0) {
            return false;
        }

        s_surface = eglCreateWindowSurface(s_display, config, win, nullptr);
        if (!s_surface) {
            return false;
        }

        static const EGLint contextAttributeList[] = {
            EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
            EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
            EGL_CONTEXT_MINOR_VERSION_KHR, 3,
            EGL_NONE
        };
        s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, contextAttributeList);
        if (!s_context) {
            eglDestroySurface(s_display, s_surface);
            s_surface = nullptr;
            return false;
        }

        eglMakeCurrent(s_display, s_surface, s_surface, s_context);
        return true;
    }

    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName, bool debug) {
        Window wind(debug);
        if (!wind.initEgl(nwindowGetDefault())) {
            return std::nullopt;
        }

        gladLoadGL();

        wind.windowWidth_ = width;
        wind.windowHeight_ = height;
        wind.debug_ = debug;

        if (debug) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

        return std::make_optional(std::move(wind));
    }

    bool Window::shouldClose() {
        return !appletMainLoop();
    }

    void Window::pollEvents() {
        Input::poll();
    }

    double Window::timer() {
        return GetSystemTimeSeconds();
    }

    void Window::swapBuffers() {
        eglSwapBuffers(s_display, s_surface);
    }

    void Window::setErrorCallback(void(*function)(int, const char*)) {
        (void)function;
    }

    float Window::getSizeRatio() {
        return windowWidth_ / (float)windowHeight_;
    }

    void Window::viewportAndClear() {
        glGetError();
        glViewport(0, 0, windowWidth_, windowHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (debug_) {
            glCheckError();
        }
    }

    void Window::loadMaterials(std::vector<Material>& materials) {
        for (auto& mat : materials) {
            if (!mat.diffuseTexPath.empty()) {

                std::string key(mat.diffuseTexPath);
                auto it = textureCache.find(key);

                if (it != textureCache.end()) {
                   mat.diffuseTexID = it->second.getId();
                    continue;
                }

                Texture tex(mat.diffuseTexPath.c_str(), debug_);

                mat.diffuseTexID = tex.getId();

                textureCache.insert({ key, std::move(tex) });
            }
        }
    }

    float Window::getLastFrameTime() {
        double currentTime = timer();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime_);
        lastFrameTime_ = currentTime;
        return deltaTime;
    }
}
