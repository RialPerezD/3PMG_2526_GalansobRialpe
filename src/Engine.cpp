
#include "Motarda/Engine.hpp"
#include <memory>


namespace MADE {

    std::optional<MotardaEng> MotardaEng::createEngine(int width, int height, const char* windowName) {
        auto w = Window::createWindow(width, height, windowName);
        if (!w) return std::nullopt;

        //Dunno why the optional don't accept the value in the ()
        MotardaEng eng = MotardaEng{ w.value() };
        return std::optional<MotardaEng>(eng);
    }


    //Default constructor
    MotardaEng::MotardaEng(Window& window) : window_{ window }{
    }


    bool MotardaEng::shouldWindowClose(){
        return window_.shouldWindowClose();
    }


    void MotardaEng::endWindowFrame() {
        window_.pollEvents();
    }


    void MotardaEng::close() {
        //TODO
    }
}