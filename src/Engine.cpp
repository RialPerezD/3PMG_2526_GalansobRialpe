
#include "Motarda/Engine.hpp"
#include <memory>


namespace MTRD {

    std::optional<MotardaEng> MotardaEng::createEngine(int width, int height, const char* windowName) {
        auto w = Window::windowCreate(width, height, windowName);
        if (!w) return std::nullopt;

        return std::make_optional(MotardaEng{ w.value() });
    }


    //Default constructor
    MotardaEng::MotardaEng(Window& window) : window_{ std::move(window) }{
    }

    bool MotardaEng::windowShouldClose(){
        return window_.shouldClose();
    }


    double MotardaEng::windowGetTimer() {
        return window_.timer();
    }


    void MotardaEng::windowEndFrame() {
        window_.swapBuffers();
        window_.pollEvents();
    }


    void MotardaEng::windowCreateContext() {
        window_.createContext();
    }


    void MotardaEng::windowSetSwapInterval(int i) {
        window_.setSwapInterval(i);
    }


    void MotardaEng::windowSetErrorCallback(void(*function)(int, const char*)) {
        window_.setErrorCallback(function);
    }


    float MotardaEng::windowGetSizeRatio() {
        return window_.getSizeRatio();
    }


    void MotardaEng::windowOpenglSetup(
        const void* vertexBuffer,
        const char* vertexShader,
        const char* fragmentShader,
        const char* uni1,
        const char* at1,
        const char* at2,
        size_t verticeSize
    ) {
        window_.openglGenerateBuffers(vertexBuffer, verticeSize);
        window_.openglGenerateVertexShaders(vertexShader);
        window_.openglGenerateFragmentShaders(fragmentShader);
        window_.openglCreateProgram();
        window_.openglSet3AtribLocations(uni1, at1, at2);
        window_.openglVertexConfig(verticeSize);
    }


    void MotardaEng::windowOpenglViewportAndClear() {
        window_.openglViewportAndClear();
    }


    void MotardaEng::windowOpenglProgramUniformDraw(const GLfloat* mvp) {
        window_.openglProgramUniformDraw(mvp);
    }


    void MotardaEng::close() {
        //TODO
    }
}