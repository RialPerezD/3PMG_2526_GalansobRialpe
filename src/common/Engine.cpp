#include "Motarda/common/Engine.hpp"
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "MotArda/win64/window.hpp"
#include <MotArda/common/Geometries.hpp>

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
        input_{ std::move(input) },
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


    void MotardaEng::windowSetErrorCallback(void(*function)(int, const char*)) {
        window_.setErrorCallback(function);
    }


    float MotardaEng::windowGetSizeRatio() {
        return window_.getSizeRatio();
    }


    void MotardaEng::windowInitFrame() {
        window_.openglViewportAndClear();
    }


    void MotardaEng::windowLoadAllMaterials(std::vector<ObjItem>& objItemsList){
        for (ObjItem& item : objItemsList) {
            window_.openglLoadMaterials(item.materials);
        }
    }


    float MotardaEng::windowGetLastFrameTime() {
        return window_.getLastFrameTime();
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


    std::vector<ObjItem> MotardaEng::loadObjs(std::vector<const char*> routes){
        std::vector<ObjItem> objItemsList = {};

        for (const char* route : routes) {
            auto maybeObjLoader = ObjLoader::loadObj(
                route,
                window_
            );

            if (!maybeObjLoader.has_value()) continue;

            ObjLoader objLoader = std::move(maybeObjLoader.value());
            ObjItem item(
                objLoader.getMeshes(),
                objLoader.getMaterials()
            );

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


    ObjItem MotardaEng::generateCube(float size, bool debug) {
		bool firstTime = false;
        return std::move(Geometries::GenerateCube(window_, size, firstTime, debug));
	}


    ObjItem MotardaEng::generatePlane(float width, float height, bool debug) {
        bool firstTime = false;
        return std::move(Geometries::GeneratePlane(window_, width, height, firstTime, debug));
    }


    ObjItem MotardaEng::generateSphere(float radius, int segments, int rings, bool debug) {
        bool firstTime = false;
        return std::move(Geometries::GenerateSphere(window_, radius, segments, rings, firstTime, debug));
    }
}