#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/common/Components/LightComponent.hpp>
#include <memory>
#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Systems/ShadowMapSystem.hpp>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}

size_t lightEntity;
glm::vec3 spotLigthCenter = glm::vec3(10, 0, 0);
glm::vec3 pointLigthCenter = glm::vec3(-10, 0, 0);
const int numInstances = 5;
const float zOffset = -20.0f;

void GenerateSpotLightEntitys(ECSManager& ecs, std::vector<MTRD::ObjItem>& objItemList) {
    for (int j = 0; j < numInstances; j++) {
        glm::vec3 currentCenter = spotLigthCenter + glm::vec3(0, 0, j * zOffset);

        size_t player = ecs.AddEntity();
        size_t floor = ecs.AddEntity();
        size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };

        MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
        t->position = glm::vec3(0, -2.f, 0) + currentCenter;
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
        r->meshes_ = &objItemList[0].meshes;
        r->materials_ = &objItemList[0].materials;

        MTRD::MovementComponent* m = ecs.AddComponent<MTRD::MovementComponent>(player);
        m->position = glm::vec3(0) + currentCenter;
        m->rotation = glm::vec3(0, 0, 1);
        m->scale = glm::vec3(0.0f);
        m->shouldConstantMove = false;

        t = ecs.AddComponent<MTRD::TransformComponent>(floor);
        t->position = glm::vec3(0, -3, 0) + currentCenter;
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        r = ecs.AddComponent<MTRD::RenderComponent>(floor);
        r->meshes_ = &objItemList[1].meshes;
        r->materials_ = &objItemList[1].materials;

        m = ecs.AddComponent<MTRD::MovementComponent>(floor);
        m->position = glm::vec3(0) + currentCenter;
        m->rotation = glm::vec3(0, 0, 1);
        m->scale = glm::vec3(0.0f);
        m->shouldConstantMove = false;

        for (int i = 0; i < 4; i++) {
            t = ecs.AddComponent<MTRD::TransformComponent>(cubes[i]);
            t->position = glm::vec3(-5 * ((i % 2) * 2 - 1), -2.0f, -5 * ((i / 2) * 2 - 1)) + currentCenter;
            t->rotation = glm::vec3(0, 0, 0);
            t->angleRotationRadians = -1;
            t->scale = glm::vec3(1.f);

            r = ecs.AddComponent<MTRD::RenderComponent>(cubes[i]);
            r->meshes_ = &objItemList[2].meshes;
            r->materials_ = &objItemList[2].materials;

            m = ecs.AddComponent<MTRD::MovementComponent>(cubes[i]);
            m->position = glm::vec3(0) + currentCenter;
            m->rotation = glm::vec3(0, 0, 1);
            m->scale = glm::vec3(0.0f);
            m->shouldConstantMove = false;
        }
    }
}

void GenerateSpotLights(MTRD::LightComponent* lightComp, MTRD::MotardaEng& eng) {
    for (int j = 0; j < numInstances; j++) {
        glm::vec3 currentCenter = spotLigthCenter + glm::vec3(0, 0, j * zOffset);

        lightComp->spotLights.push_back(
            MTRD::SpotLight(
                glm::vec3(0.0f, 0.0f, 0.0f) + currentCenter,
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                5.0f, 1.f, 0.75f, 1.0f, 0.09f, 0.032f, eng.windowGetSizeRatio()
            )
        );

        lightComp->spotLights.push_back(
            MTRD::SpotLight(
                glm::vec3(0.0f, 0.0f, 0.0f) + currentCenter,
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                5.0f, 1.0f, 0.75f, 1.0f, 0.09f, 0.032f, eng.windowGetSizeRatio()
            )
        );

        lightComp->spotLights.push_back(
            MTRD::SpotLight(
                glm::vec3(5.0f, 5.0f, 5.0f) + currentCenter,
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                7.0f, 1.0f, 0.85f, 1.0f, 0.09f, 0.032f, eng.windowGetSizeRatio()
            )
        );
    }
}

void GeneratePointLightEntitys(ECSManager& ecs, std::vector<MTRD::ObjItem>& objItemList) {
    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };
    lightEntity = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, -2.f, 0) + pointLigthCenter;
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->meshes_ = &objItemList[0].meshes;
    r->materials_ = &objItemList[0].materials;

    MTRD::MovementComponent* m = ecs.AddComponent<MTRD::MovementComponent>(player);
    m->position = glm::vec3(0) + pointLigthCenter;
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;

    t = ecs.AddComponent<MTRD::TransformComponent>(floor);
    t->position = glm::vec3(0, -3, 0) + pointLigthCenter;
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    r = ecs.AddComponent<MTRD::RenderComponent>(floor);
    r->meshes_ = &objItemList[1].meshes;
    r->materials_ = &objItemList[1].materials;

    m = ecs.AddComponent<MTRD::MovementComponent>(floor);
    m->position = glm::vec3(0) + pointLigthCenter;
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;

    for (int i = 0; i < 4; i++) {
        t = ecs.AddComponent<MTRD::TransformComponent>(cubes[i]);
        t->position = glm::vec3(-5 * ((i % 2) * 2 - 1), -2.0f, -5 * ((i / 2) * 2 - 1)) + pointLigthCenter;
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        r = ecs.AddComponent<MTRD::RenderComponent>(cubes[i]);
        r->meshes_ = &objItemList[2].meshes;
        r->materials_ = &objItemList[2].materials;

        m = ecs.AddComponent<MTRD::MovementComponent>(cubes[i]);
        m->position = glm::vec3(0) + pointLigthCenter;
        m->rotation = glm::vec3(0, 0, 1);
        m->scale = glm::vec3(0.0f);
        m->shouldConstantMove = false;
    }
}

void GeneratePointLights(MTRD::LightComponent* lightComp, MTRD::MotardaEng& eng) {
    lightComp->pointLights.push_back(
        MTRD::PointLight(
            glm::vec3(0.0f, 1.0f, 0.0f) + pointLigthCenter,
            glm::vec3(1.0f, 1.0f, 0.0f),
            1.0f, 1.0f, 0.09f, 0.032f
        )
    );
}

void GenerateDirectionalLights(MTRD::LightComponent* lightComp, MTRD::MotardaEng& eng) {
    lightComp->directionalLights.push_back(
        MTRD::DirectionalLight(
            glm::vec3(-1.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 1.0f),
            0.05f
        )
    );
}

int MTRD::main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 1, 20));
    camera.setTarget(glm::vec3(0.0f, -5.0f, 0.0f));
    float movSpeed = 0.5f;

    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::DeferredWithLights);
    eng.windowSetErrorCallback(error_callback);

    std::vector<ObjItem> objItemList;
    objItemList.push_back(std::move(eng.generateSphere(0.5f, 20, 20, 1)));
    objItemList.push_back(std::move(eng.generatePlane(20, 20, 1)));
    objItemList.push_back(std::move(eng.generateCube(1)));
    eng.windowLoadAllMaterials(objItemList);

    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();
    ecs.AddComponentType<MTRD::LightComponent>();

    GenerateSpotLightEntitys(ecs, objItemList);
    GeneratePointLightEntitys(ecs, objItemList);

    MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(lightEntity);
    GenerateSpotLights(lightComp, eng);
    GenerateDirectionalLights(lightComp, eng);
    GeneratePointLights(lightComp, eng);

    float radio = 10.f;
    float velocidad = 1.f;
    float timer = 0.0f;

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (eng.inputIsKeyPressed(Input::Keyboard::W)) camera.moveForward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::S)) camera.moveBackward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) camera.moveLeft(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) camera.moveRight(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::R)) camera.moveUp(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::T)) camera.moveDown(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::E)) camera.rotate(10.0f, 0.0f);
        if (eng.inputIsKeyPressed(Input::Keyboard::Q)) camera.rotate(-10.0f, 0.0f);
        if (eng.inputIsKeyPressed(Input::Keyboard::F)) camera.rotate(0.0f, 10.0f);
        if (eng.inputIsKeyPressed(Input::Keyboard::G)) camera.rotate(0.0f, -10.0f);

        timer += eng.windowGetLastFrameTime();

        float posX2 = radio * sin(timer * velocidad);
        float posY2 = radio * cos(timer * velocidad);

        for (int j = 0; j < numInstances; j++) {
            glm::vec3 currentCenter = spotLigthCenter + glm::vec3(0, 0, j * zOffset);
            int idx = j * 3;

            float posX = radio * cos(timer * velocidad + idx);
            float posY = radio * sin(timer * velocidad + idx);

            lightComp->spotLights[idx].position_ = glm::vec3(posX / 5, 0.0f, posY / 5) + currentCenter;
            lightComp->spotLights[idx].direction_ = glm::normalize(glm::vec3(0, -2, 0) - lightComp->spotLights[idx].position_ + currentCenter);

            lightComp->spotLights[idx + 1].position_ = glm::vec3(posX2 / 5, 0.0f, posY2 / 5) + currentCenter;
            lightComp->spotLights[idx + 1].direction_ = glm::normalize(glm::vec3(0, -2, 0) - lightComp->spotLights[idx + 1].position_ + currentCenter);

            lightComp->spotLights[idx + 2].position_ = glm::vec3(-posX * 0.68f, 0.0f, posY * 0.68f) + currentCenter;
        }

        eng.RenderScene();
        eng.windowEndFrame();
    }

    return 0;
}