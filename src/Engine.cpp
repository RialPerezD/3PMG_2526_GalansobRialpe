
#include "Motarda/Engine.hpp"
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "MotArda/window.hpp"

namespace MTRD {

    std::optional<MotardaEng> MotardaEng::createEngine(int width, int height, const char* windowName) {
        auto w = Window::windowCreate(width, height, windowName);
        if (!w) return std::nullopt;

        Input input_ = Input::inputCreate(w.value());

        JobSystem js;

        return std::make_optional<MotardaEng>(MotardaEng{ std::move(w.value()), std::move(input_), std::move(js) });
    }

    //Default constructor
    MotardaEng::MotardaEng(Window window, Input input, JobSystem js)
        :
        window_{ std::move(window) },
        input_{ std::move(input_) },
        jobSystem_{ std::move(js) }
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


    void MotardaEng::windowNoObjectsOpenglSetup(
        const char* vertexShader,
        const char* fragmentShader,
        std::vector<Window::UniformAttrib>& uniforms,
        const std::vector<Window::VertexAttrib>& attributes
    ) {
        window_.openglGenerateVertexShaders(vertexShader);
        window_.openglGenerateFragmentShaders(fragmentShader);
        window_.openglCreateProgram();
        window_.openglSetUniformsLocationsAndAtributtes(uniforms, attributes);
    }


    void MotardaEng::windowOpenglSetup(
        std::vector<std::pair<size_t, Render>>& renderComponents,
        const char* vertexShader,
        const char* fragmentShader,
        std::vector<Window::UniformAttrib>& uniforms,
        const std::vector<Window::VertexAttrib>& attributes
    ) {

        window_.openglGenerateVertexShaders(vertexShader);
        window_.openglGenerateFragmentShaders(fragmentShader);
        window_.openglCreateProgram();

        if (renderComponents.size() == 0) return;
        updateVertexBuffers(
            renderComponents,
            uniforms,
            attributes
        );
    }


    void MotardaEng::windowInitFrame() {
        window_.openglViewportAndClear();
    }


    void MotardaEng::windowOpenglSetUniformsValues(std::vector<Window::UniformAttrib> uniforms) {
        window_.openglSetUniformsValues(uniforms);
    }


    void MotardaEng::windowOpenglProgramUniformDrawRender(Render& renderComponents) {
        window_.openglProgramUniformDraw(renderComponents);
    }


    void MotardaEng::windowSetDebugMode(bool b) {
        window_.setDebugMode(b);
    }


    void MotardaEng::windowLoadAllMaterials(std::vector<MTRD::Window::ObjItem>& objItemsList){
        for (MTRD::Window::ObjItem& item : objItemsList) {
            windowLoadMaterials(item.materials);
        }
    }


    float MotardaEng::windowGetLastFrameTime() {
        return window_.getLastFrameTime();
    }


    void MotardaEng::windowLoadMaterials(std::vector<Material>& materials) {
        window_.openglLoadMaterials(materials);
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


    void MotardaEng::enqueueTask(std::function<void()> task){
        jobSystem_.enqueue(task);
    }


    std::vector<MTRD::Window::ObjItem> MotardaEng::loadObjs(std::vector<const char*> routes){
        std::vector<MTRD::Window::ObjItem> objItemsList = {};

        for (const char* route : routes) {
            auto maybeObjLoader = ObjLoader::loadObj(
                route
            );

            if (!maybeObjLoader.has_value()) continue;

            ObjLoader objLoader = maybeObjLoader.value();
            MTRD::Window::ObjItem item(objLoader.getShapes(), objLoader.getMaterials());

            objItemsList.push_back(std::move(item));
        }

        return objItemsList;
    }


    const char* MotardaEng::loadShaderFile(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: no se pudo abrir el archivo " << filename << std::endl;
            return nullptr;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string* shaderSource = new std::string(buffer.str());

        return shaderSource->c_str();
    }


    void MotardaEng::updateVertexBuffers(
        std::vector<std::pair<size_t, Render>>& renderComponents,
        std::vector<Window::UniformAttrib>& uniforms,
        const std::vector<Window::VertexAttrib>& attributes
    ) {
        for (int i = 0; i < renderComponents.size(); i++) {
            Render* render = &renderComponents[i].second;
            for (int j = 0; j < render->shapes->size(); j++) {
                std::vector<Vertex> vertexes = render->shapes->at(j).vertices;
                const void* vertexBuffer = static_cast<const void*> (vertexes.data());

                window_.openglClearVertexBuffers(render->shapes->at(j).vao);
                window_.openglGenerateVertexBuffers(
                    vertexBuffer,
                    static_cast<int>(vertexes.size()),
                    render->shapes->at(j).vao
                );
                window_.openglSetUniformsLocationsAndAtributtes(uniforms, attributes);
            }
        }
    }
}