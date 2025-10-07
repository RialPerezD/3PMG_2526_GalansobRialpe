
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


    void MotardaEng::windowEndFrame() {
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


    void MotardaEng::windowOpenglGenerateBuffers(const void* vertex) {
        window_.openglGenerateBuffers(vertex);
    }


    void MotardaEng::windowOpenglGenerateVertexShaders(const char* text) {
        window_.openglGenerateVertexShaders(text);
    }


    void MotardaEng::windowOpenglGenerateFragmentShaders(const char* text) {
        window_.openglGenerateFragmentShaders(text);
    }


    void MotardaEng::windowOpenglCreateProgram() {
        window_.openglCreateProgram();
    }


    void MotardaEng::close() {
        //TODO
    }
}