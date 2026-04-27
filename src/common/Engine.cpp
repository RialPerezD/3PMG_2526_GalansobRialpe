#include <enet/enet.h>
#include "Motarda/common/Engine.hpp"
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "MotArda/win64/window.hpp"
#include <MotArda/common/Geometries.hpp>
#include <MotArda/win64/Systems/RenderPbrSystem.hpp>
#include "MotArda/common/Logger.hpp"

namespace MTRD {
    std::optional<MotardaEng> MotardaEng::createEngine(
        int width,
        int height,
        const char* windowName
    ) {
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
        jobSystem_{ std::move(js) },
        vp_ (glm::mat4(1.0f)),
        model_ (glm::mat4(1.0f)),
        camera_(Camera::CreateCamera(windowGetSizeRatio())),
        initialized2D (false),
        hasPhysx_(false),
        basePlane_ (std::move(generatePlane(20, 20))),
        debug_ (true)
    {
        input_.generateAsciiMap();
        input_.setWindow(&window_);
    }


    void MotardaEng::SetDebugMode(bool debug) {
        debug_ = debug;
        window_.setDebugMode(debug);
    }


    bool MotardaEng::windowShouldClose(){
        return window_.shouldClose();
    }


    double MotardaEng::windowGetTimer() {
        return window_.timer();
    }


    void MotardaEng::windowEndFrame() {
        window_.swapBuffers();
        window_.imGuiEndFrame();

        input_.clearBuffers();
        input_.clearMouseBuffers();
    }


    void MotardaEng::windowSetErrorCallback(void(*function)(int, const char*)) {
        window_.setErrorCallback(function);
    }


    float MotardaEng::windowGetSizeRatio() {
        return window_.getSizeRatio();
    }


    void MotardaEng::windowInitFrame() {
        window_.viewportAndClear();
        window_.pollEvents();
    }


    void MotardaEng::windowLoadAllMaterials(std::vector<ObjItem>& objItemsList){
        for (ObjItem& item : objItemsList) {
            window_.loadMaterials(item.materials);
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


    bool MotardaEng::inputIsMouseButtonPressed(Input::MouseButton button) {
        return input_.isMouseButtonPressed(button);
    }

    bool MotardaEng::inputIsMouseButtonDown(Input::MouseButton button) {
        return input_.isMouseButtonDown(button);
    }

    void MotardaEng::inputGetMousePosition(int& x, int& y) {
        input_.getMousePosition(x, y);
    }

    glm::vec3 MotardaEng::raycastFromMouse(float maxDistance) {
        int mouseX, mouseY;
        input_.setWindow(&window_);
        input_.getMousePosition(mouseX, mouseY);

        int width = window_.getWidth();
        int height = window_.getHeight();

        float ndcX = (2.0f * mouseX) / width - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / height;

        glm::mat4 invProj = glm::inverse(camera_.getProjection());
        glm::mat4 invView = glm::inverse(camera_.getView());

        glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = invProj * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec3 rayWorld = glm::vec3(invView * rayEye);
        rayWorld = glm::normalize(rayWorld);

        if (hasPhysx_ && physx_.initialized) {
            glm::vec3 hitPosition;
            void* hitActor;
            if (physx_.raycast(camera_.getPosition(), rayWorld, maxDistance, hitPosition, hitActor)) {
                return hitPosition;
            }
        }

        return camera_.getPosition() + rayWorld * maxDistance;
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
            //std::cerr << "Error: no se pudo abrir el archivo " << filename << std::endl;
            MTRD::Logger::error("No se pudo abrir el archivo");
            return nullptr;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string* shaderSource = new std::string(buffer.str());

        return shaderSource->c_str();
    }


    std::unique_ptr<Mesh> MotardaEng::createMesh(
        std::vector<Vertex> vertices,
        std::string name) {

        bool firstTime = false;

        return std::make_unique<Mesh>(
            vertices,
            window_,
            name,
            firstTime,
            -1,
            debug_
        );
    }


    ObjItem MotardaEng::generateCube(float size, int texureId, bool debug) {
		bool firstTime = false;
        return std::move(Geometries::GenerateCube(window_, size, firstTime, texureId, debug));
	}


    ObjItem MotardaEng::generatePlane(float width, float height, int texureId, bool debug) {
        bool firstTime = false;
        return std::move(Geometries::GeneratePlane(window_, width, height, firstTime, texureId, debug));
    }


    ObjItem MotardaEng::generateSphere(float radius, int segments, int rings, int texureId, bool debug) {
        bool firstTime = false;
        return std::move(Geometries::GenerateSphere(window_, radius, segments, rings, firstTime, texureId, debug));
    }


    Sprite& MotardaEng::generateSprite(GLuint textureIndex, float size, float deep) {

        if (!initialized2D) {
            initialized2D = true;

            ecs_.AddComponentType<MTRD::TransformComponent>();
            ecs_.AddComponentType<MTRD::RenderComponent>();
        }
        size_t spriteId = ecs_.AddEntity();

        TransformComponent* t = ecs_.AddComponent<TransformComponent>(spriteId);
		t->position = glm::vec3(0, 0, 0.1f * deep);
        t->scale = glm::vec3(size * 0.05f);
        t->rotation = glm::vec3( 1, 0, 0 );
        t->angleRotationRadians = 1.5708f;

        RenderComponent* r = ecs_.AddComponent<MTRD::RenderComponent>(spriteId);
        r->meshes_ = &basePlane_.meshes;
        r->materials_->emplace_back();

        r->materials_[0][0].diffuseTexID = textureIndex;

		Sprite sprite(spriteId, deep);

        return sprite;
    }

    Sprite& MotardaEng::generateSpriteSheet(GLuint textureIndex, float size, int frameWidth, int frameHeight, int columns, int rows, float deep) {
        if (!initialized2D) {
            initialized2D = true;

            ecs_.AddComponentType<MTRD::TransformComponent>();
            ecs_.AddComponentType<MTRD::RenderComponent>();
        }
        size_t spriteId = ecs_.AddEntity();

        TransformComponent* t = ecs_.AddComponent<TransformComponent>(spriteId);
		t->position = glm::vec3(0, 0, 0.1f * deep);
        t->scale = glm::vec3(size * 0.05f);
        t->rotation = glm::vec3( 1, 0, 0 );
        t->angleRotationRadians = 1.5708f;

        RenderComponent* r = ecs_.AddComponent<MTRD::RenderComponent>(spriteId);
        r->meshes_ = &basePlane_.meshes;
        r->materials_->emplace_back();

        r->materials_[0][0].diffuseTexID = textureIndex;

        Sprite sprite(spriteId, deep);
        sprite.setRenderComponent(r);
        sprite.setFrameSize(frameWidth, frameHeight);
        
        int totalFrames = columns * rows;
        sprite.totalFrames_ = totalFrames;
        sprite.columns_ = columns;
        sprite.rows_ = rows;
        sprite.setFrame(0);

        return sprite;
    }


    void MotardaEng::createPhysxActor(
        MTRD::PhysxComponent& p,
        MTRD::TransformComponent& t
    ) {
        physx_.createActor(&p, &t);
    }


    void MotardaEng::hasPhysx(bool has) {
        hasPhysx_ = has;

        if (!physx_.initialized) {
			physx_.init();
        }
	}


    void MotardaEng::SetRenderType(RenderType type){
        actualRenderType_ = type;

        switch (type) {
            case RenderType::Base:
                renderSystem_ = std::make_unique<RenderSystem>(vp_, model_, debug_);
                break;
            case RenderType::Lights:
                renderLightsSystem_ = std::make_unique<RenderLightsSystem>(vp_, model_, camera_.getPosition(), debug_, window_.getWidth(), window_.getHeight());
                break;
            case RenderType::LightsWithShadows:
                renderLightsSystem_ = std::make_unique<RenderLightsSystem>(vp_, model_, camera_.getPosition(), debug_, window_.getWidth(), window_.getHeight());
                shadowSystem_ = std::make_unique<ShadowMapSystem>(model_, debug_);
                break;
            case RenderType::Bidimensional:
                renderSystem_ = std::make_unique<RenderSystem>(vp_, model_, debug_);
                break;
            case RenderType::DeferredWithLights:
                defferredSystem_ = std::make_unique<RenderDefferredSystem>(vp_, model_, camera_.getPosition(), debug_, window_.getWidth(), window_.getHeight());
                shadowSystem_ = std::make_unique<ShadowMapSystem>(model_, debug_);
                break;
            case RenderType::Pbr:
                pbrSystem_ = std::make_unique<RenderPbrSystem>(vp_, model_, camera_.getPosition(), debug_, window_.getWidth(), window_.getHeight());
                shadowSystem_ = std::make_unique<ShadowMapSystem>(model_, debug_);
                break;
		}
    }


    void MotardaEng::RenderScene() {
        vp_ = camera_.getViewProj();

        switch (actualRenderType_) {
        case RenderType::Base:
            if (!renderSystem_) {
                //printf("There are no render system");
                MTRD::Logger::error("There are no render system");
                return;
            }
            renderSystem_->Render(ecs_, model_);
            break;

        case RenderType::Lights:
            if (!renderLightsSystem_) {
                //printf("There are no light render system");
                MTRD::Logger::error("There are no light system");

                return;
            }
            renderLightsSystem_->Render(ecs_, model_);
            break;

        case RenderType::LightsWithShadows:
            if (!shadowSystem_ || !renderLightsSystem_) {
                //printf("There are no light or shadow render system");
                MTRD::Logger::error("There are no light or shadow render system");


                return;
            }
            shadowSystem_->RenderShadowMap(ecs_, model_);
            renderLightsSystem_->SetShadowMaps(shadowSystem_->getAllDepthMaps());
            renderLightsSystem_->SetShadowCubemaps(shadowSystem_->getAllDepthCubemaps());
            renderLightsSystem_->Render(ecs_, model_, true);
            break;

        case RenderType::Bidimensional:
            if (!renderSystem_) {
                //printf("There are no 2d render system");
                MTRD::Logger::error("There are no 2d render system");

                return;
            }
            renderSystem_->Render(ecs_, model_);
            break;

        case RenderType::DeferredWithLights:
            if (!defferredSystem_) {
                //printf("There are no deferred render system");
                MTRD::Logger::error("There are no deferred render system");

                return;
            }
            shadowSystem_->RenderShadowMap(ecs_, model_);

            defferredSystem_->SetShadowMaps(shadowSystem_->getAllDepthMaps());
            defferredSystem_->SetShadowCubemaps(shadowSystem_->getAllDepthCubemaps());

            defferredSystem_->Render(ecs_, model_, true);
            break;

        case RenderType::Pbr:
            if (!pbrSystem_) {
                //printf("There are no PBR render system");
                MTRD::Logger::error("There are no render system");

                return;
            }
            shadowSystem_->RenderShadowMap(ecs_, model_);

            pbrSystem_->SetShadowMaps(shadowSystem_->getAllDepthMaps());
            pbrSystem_->SetShadowCubemaps(shadowSystem_->getAllDepthCubemaps());

            pbrSystem_->Render(ecs_, model_, true);
            break;
        }

        window_.imGuiRender();


        // --- Physics update ---
        if (hasPhysx_) {
            float deltaTime = windowGetLastFrameTime();
            physx_.update(deltaTime);

            MTRD::TransformComponent* t;
            MTRD::PhysxComponent* p;
            // Sync physics to transforms
            for (size_t id : ecs_.GetEntitiesWithComponents<MTRD::TransformComponent, MTRD::PhysxComponent>()) {
                t = ecs_.GetComponent<MTRD::TransformComponent>(id);
                p = ecs_.GetComponent<MTRD::PhysxComponent>(id);
                physx_.syncTransform(p, t);
            }
        }
        // --- *** ---
    }
}