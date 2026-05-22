#pragma once

#include <MotArda/Texture.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/ObjItem.hpp>

#include <optional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include <glad/glad.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <switch.h>

namespace MTRD {
    class Mesh;
    class Material;

    class Window {
    public:
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
            const char* windowName,
            bool debug = false);

        ~Window();

        Window(Window&& right);
        Window& operator=(Window&& right) = default;

        struct UniformAttrib {
            const char* name;
            GLint location;
            UniformTypes type;
            float* values;
        };

        bool shouldClose();
        void pollEvents();
        double timer();
        void swapBuffers();
        void setErrorCallback(void(*function)(int, const char*));
        float getSizeRatio();
        int getWidth() const { return windowWidth_; }
        int getHeight() const { return windowHeight_; }
        float getLastFrameTime();

        void viewportAndClear();
        void loadMaterials(std::vector<Material>& materials);

        void imGuiRender() {}
        void imGuiEndFrame() {}

        void setDebugMode(bool debug) { debug_ = debug; }

    private:
        explicit Window(bool debug);

        bool initEgl(NWindow* win);
        void checkErrors();

        int s_nxlinkSock;

        EGLDisplay s_display;
        EGLContext s_context;
        EGLSurface s_surface;

        int windowWidth_;
        int windowHeight_;
        bool debug_;
        double lastFrameTime_ = 0.0;

        std::unordered_map<std::string, Texture> textureCache;
    };
}
