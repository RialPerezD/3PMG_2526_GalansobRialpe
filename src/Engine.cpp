
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
        return window_.windowShouldClose();
    }


    void MotardaEng::windowEndFrame() {
        window_.windowPollEvents();
    }


    void MotardaEng::windowCreateContext() {
        window_.windowCreateContext();
    }


    void MotardaEng::windowSetSwapInterval(int i) {
        window_.windowSetSwapInterval(i);
    }


    void MotardaEng::close() {
        //TODO
    }
}