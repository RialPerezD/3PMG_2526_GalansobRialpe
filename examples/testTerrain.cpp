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
    eng.SetRenderType(MotardaEng::RenderType::Base);
    eng.windowSetErrorCallback(error_callback);

    std::vector<ObjItem> objItemList;
    objItemList.push_back(std::move(eng.generateSphere(0.5f, 20, 20, 0)));
    eng.windowLoadAllMaterials(objItemList);


    ECSManager& ecs = eng.getEcs();

    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t player = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, -2.5f, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->meshes_ = &objItemList[0].meshes;
    r->materials_ = &objItemList[0].materials;


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

        eng.RenderScene();
        eng.windowEndFrame();
    }

    return 0;
}