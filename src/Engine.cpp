
#include "Motarda/Engine.hpp"
#include <memory>


namespace MTRD {

    std::optional<MotardaEng> MotardaEng::createEngine(int width, int height, const char* windowName) {
        auto w = Window::windowCreate(width, height, windowName);
        if (!w) return std::nullopt;

        Input input_ = Input::inputCreate(w.value());

        return std::make_optional(MotardaEng{ w.value(), input_ });
    }


    //Default constructor
    MotardaEng::MotardaEng(Window& window, Input& input)
        :
        window_{ std::move(window) },
        input_{ std::move(input_) }
    {
        input_.generateAsciiMap();
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
        const std::vector<const char*>& uniforms,
        const std::vector<Window::VertexAttrib>& attributes,
        size_t verticeSize,
        int numVertex
    ) {
        window_.openglGenerateBuffers(vertexBuffer, verticeSize, numVertex);
        window_.openglGenerateVertexShaders(vertexShader);
        window_.openglGenerateFragmentShaders(fragmentShader);
        window_.openglCreateProgram();

        window_.openglSetAttributesAndUniforms(uniforms, attributes, verticeSize);
    }


    void MotardaEng::windowInitFrame() {
        window_.openglViewportAndClear();
    }


    void MotardaEng::windowOpenglProgramUniformDraw(const GLfloat* mvp, int ammountPoints) {
        window_.openglProgramUniformDraw(mvp, ammountPoints);
    }

    void MotardaEng::windowSetDebugMode(bool b) {
        window_.setDebugMode(b);
    }


    void MotardaEng::close() {
        //TODO
    }


    bool MotardaEng::inputIsKeyPressed(Input::Keyboard key) {
        return input_.isKeyPressed(key);
    }


    bool MotardaEng::inputIsKeyDown(Input::Keyboard key) {
        return input_.isKeyDown(key);
    }


    bool MotardaEng::inputIsKeyUp(Input::Keyboard key) {
        return input_.isKeyUp(key);
    }
}