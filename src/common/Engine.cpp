#ifndef __SWITCH__
#include <enet/enet.h>
#endif
#include <Motarda/Engine.hpp>
#include <MotArda/window.hpp>
#include <MotArda/Geometries.hpp>
#include <MotArda/Terrain.hpp>
#include <MotArda/Systems/RenderPbrSystem.hpp>
#ifndef __SWITCH__
#include <MotArda/Logger.hpp>
#include <MotArda/Logger.hpp>
#endif

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>


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
        debug_(true),
#ifndef __SWITCH__
        online_(false),
#endif
        camera_(Camera::CreateCamera(windowGetSizeRatio())),
        vp_(glm::mat4(1.0f)),
        model_(glm::mat4(1.0f)),
        initialized2D(false),
        basePlane_(generatePlane(20, 20))
#ifndef __SWITCH__
        , hasPhysx_(false)
#endif
    {
        input_.generateAsciiMap();
        input_.setWindow(&window_);
    }


    void MotardaEng::SetDebugMode(bool debug) {
        debug_ = debug;
        window_.setDebugMode(debug);
    }


    bool MotardaEng::windowShouldClose() {
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


    void MotardaEng::windowLoadAllMaterials(std::vector<std::shared_ptr<ObjItem>>& objItemsList) {
        for (auto& item : objItemsList) {
            window_.loadMaterials(item->materials);
        }
    }


    float MotardaEng::windowGetLastFrameTime() {
        return window_.getLastFrameTime();
    }


    bool MTRD::MotardaEng::inputIsKeyPressed(Input::Keyboard key) {
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

#ifndef __SWITCH__
        if (hasPhysx_ && physx_.initialized) {
            glm::vec3 hitPosition;
            void* hitActor;
            if (physx_.raycast(camera_.getPosition(), rayWorld, maxDistance, hitPosition, hitActor)) {
                return hitPosition;
            }
        }
#endif

        return camera_.getPosition() + rayWorld * maxDistance;
    }


    void MotardaEng::enqueueTask(std::function<void()> task) {
        jobSystem_.enqueue(task);
    }


    std::vector<std::shared_ptr<ObjItem>> MotardaEng::loadObjs(const std::vector<const char*>& routes) {
        std::vector<std::shared_ptr<ObjItem>> objItemsList = {};

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

            objItemsList.push_back(std::make_shared<ObjItem>(std::move(item)));
        }

        return objItemsList;
    }


    const char* MotardaEng::loadShaderFile(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
#ifndef __SWITCH__
            MTRD::Logger::error("No se pudo abrir el archivo");
#endif
            return nullptr;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        static std::string shaderSource;
        shaderSource = buffer.str();

        return shaderSource.c_str();
    }


    std::unique_ptr<Mesh> MotardaEng::createMesh(
        const std::vector<Vertex>& vertices,
        const std::string& name) {

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


    std::shared_ptr<ObjItem> MotardaEng::generateCube(float size, int texureId, bool debug) {
        bool firstTime = false;
        return std::make_shared<ObjItem>(Geometries::GenerateCube(window_, size, firstTime, texureId, debug));
    }


    std::shared_ptr<ObjItem> MotardaEng::generatePlane(float width, float height, int texureId, bool debug) {
        bool firstTime = false;
        return std::make_shared<ObjItem>(Geometries::GeneratePlane(window_, width, height, firstTime, texureId, debug));
    }


    std::shared_ptr<ObjItem> MotardaEng::generateSphere(float radius, int segments, int rings, int texureId, bool debug) {
        bool firstTime = false;
        return std::make_shared<ObjItem>(Geometries::GenerateSphere(window_, radius, segments, rings, firstTime, texureId, debug));
    }


    std::unique_ptr<Terrain> MotardaEng::CreateTerrain(float width, float depth, float maxHeight, int seed, int textureId, bool debug) {
        
        if(seed == -1) {
            seed = std::rand();
		}

		auto terrain = std::make_unique<Terrain>(width, depth, maxHeight, seed);

        bool firstTime = false;
		terrain.get()->GenerateProcedural(256, window_, firstTime, textureId, debug);

        return terrain;
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
        t->rotation = glm::vec3(1, 0, 0);
        t->angleRotationRadians = 1.5708f;

        RenderComponent* r = ecs_.AddComponent<MTRD::RenderComponent>(spriteId);
        r->objitem_ = generatePlane(1, 1, 0, debug_);
        r->objitem_->materials.clear();
        r->objitem_->materials.emplace_back();
        r->objitem_->materials[0].diffuseTexID = textureIndex;


        sprites_.emplace_back(spriteId, deep);

        return sprites_.back();
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
        t->rotation = glm::vec3(1, 0, 0);
        t->angleRotationRadians = 1.5708f;

        RenderComponent* r = ecs_.AddComponent<MTRD::RenderComponent>(spriteId);
        r->objitem_ = generatePlane(1, 1, 0, debug_);
        r->objitem_->materials.clear();
        r->objitem_->materials.emplace_back();
        r->objitem_->materials[0].diffuseTexID = textureIndex;

        sprites_.emplace_back(spriteId, deep);
        Sprite& sprite = sprites_.back();

        sprite.setRenderComponent(r);
        sprite.setFrameSize(frameWidth, frameHeight);

        int totalFrames = columns * rows;
        sprite.totalFrames_ = totalFrames;
        sprite.columns_ = columns;
        sprite.rows_ = rows;
        sprite.setFrame(0);

        return sprite;
    }


#ifndef __SWITCH__
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
#endif


    void MotardaEng::SetRenderType(RenderType type) {
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
#ifndef __SWITCH__
                MTRD::Logger::error("There are no render system");
#endif
                return;
            }
            renderSystem_->Render(ecs_, model_);
            break;

        case RenderType::Lights:
            if (!renderLightsSystem_) {
#ifndef __SWITCH__
                MTRD::Logger::error("There are no light system");
#endif

                return;
            }
            renderLightsSystem_->Render(ecs_, model_);
            break;

        case RenderType::LightsWithShadows:
            if (!shadowSystem_ || !renderLightsSystem_) {
#ifndef __SWITCH__
                MTRD::Logger::error("There are no light or shadow render system");
#endif


                return;
            }
            shadowSystem_->RenderShadowMap(ecs_, model_);
            renderLightsSystem_->SetShadowMaps(shadowSystem_->getAllDepthMaps());
            renderLightsSystem_->SetShadowCubemaps(shadowSystem_->getAllDepthCubemaps());
            renderLightsSystem_->Render(ecs_, model_, true);
            break;

        case RenderType::Bidimensional:
            if (!renderSystem_) {
#ifndef __SWITCH__
                MTRD::Logger::error("There are no 2d render system");
#endif

                return;
            }
            renderSystem_->Render(ecs_, model_);
            break;

        case RenderType::DeferredWithLights:
            if (!defferredSystem_) {
#ifndef __SWITCH__
                MTRD::Logger::error("There are no deferred render system");
#endif

                return;
            }
            shadowSystem_->RenderShadowMap(ecs_, model_);

            defferredSystem_->SetShadowMaps(shadowSystem_->getAllDepthMaps());
            defferredSystem_->SetShadowCubemaps(shadowSystem_->getAllDepthCubemaps());

            defferredSystem_->Render(ecs_, model_, true);
            break;

        case RenderType::Pbr:
            if (!pbrSystem_) {
#ifndef __SWITCH__
                MTRD::Logger::error("There are no render system");
#endif

                return;
            }
            shadowSystem_->RenderShadowMap(ecs_, model_);

            pbrSystem_->SetShadowMaps(shadowSystem_->getAllDepthMaps());
            pbrSystem_->SetShadowCubemaps(shadowSystem_->getAllDepthCubemaps());

            pbrSystem_->Render(ecs_, model_, true);
            break;
        }

        window_.imGuiRender();


#ifndef __SWITCH__
        // --- Physics update ---
        if (hasPhysx_) {
            float deltaTime = windowGetLastFrameTime();
            physx_.update(deltaTime);

            auto& physxList = ecs_.GetComponentList<MTRD::PhysxComponent>();
            for (auto& [id, p] : physxList) {
                MTRD::TransformComponent* t = ecs_.GetComponent<MTRD::TransformComponent>(id);
                if (t != nullptr) {
                    physx_.syncTransform(&p, t);
                }
            }
        }
        // --- *** ---
#endif
    }
}