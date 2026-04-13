#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/common/Components/LightComponent.hpp>
#include <MotArda/common/Components/PhysxComponent.hpp>

#include <memory>
#include <iostream>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}


int MTRD::main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda Mouse Raycast");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 5, 15));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    float movSpeed = 0.1f;

    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Base);
    eng.hasPhysx(true);
    eng.windowSetErrorCallback(error_callback);

    std::vector<ObjItem> objItemList;
    objItemList.push_back(std::move(eng.generateSphere(0.5f, 20, 20)));
    objItemList.push_back(std::move(eng.generatePlane(20, 20)));
    objItemList.push_back(std::move(eng.generateCube(1)));
    eng.windowLoadAllMaterials(objItemList);

    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();
    ecs.AddComponentType<MTRD::LightComponent>();
    ecs.AddComponentType<MTRD::PhysxComponent>();

    size_t floor = ecs.AddEntity();
    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(floor);
    t->position = glm::vec3(0, -2, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(floor);
    r->meshes_ = &objItemList[1].meshes;
    r->materials_ = &objItemList[1].materials;

    MTRD::PhysxComponent* floorPhysx = ecs.AddComponent<MTRD::PhysxComponent>(floor);
    floorPhysx->shapeType = MTRD::PhysxShapeType::Box;
    floorPhysx->halfExtents = glm::vec3(10.0f, 0.05f, 10.0f);
    floorPhysx->mass = 0.0f;
    floorPhysx->isDynamic = false;
    eng.createPhysxActor(*floorPhysx, *t);

    size_t box = ecs.AddEntity();
    t = ecs.AddComponent<MTRD::TransformComponent>(box);
    t->position = glm::vec3(0, 2, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    r = ecs.AddComponent<MTRD::RenderComponent>(box);
    r->meshes_ = &objItemList[2].meshes;
    r->materials_ = &objItemList[2].materials;

    MTRD::PhysxComponent* boxPhysx = ecs.AddComponent<MTRD::PhysxComponent>(box);
    boxPhysx->shapeType = MTRD::PhysxShapeType::Box;
    boxPhysx->halfExtents = glm::vec3(0.5f, 0.5f, 0.5f);
    boxPhysx->mass = 1.0f;
    boxPhysx->isDynamic = true;
    eng.createPhysxActor(*boxPhysx, *t);

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (eng.inputIsKeyPressed(Input::Keyboard::W)) camera.moveForward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::S)) camera.moveBackward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) camera.moveLeft(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) camera.moveRight(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::R)) camera.moveUp(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::T)) camera.moveDown(movSpeed);

        if (eng.inputIsMouseButtonPressed(Input::MouseButton::Left)) {
            glm::vec3 hitPoint = eng.raycastFromMouse(100.0f);
            std::cout << "Raycast hit: (" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")" << std::endl;
        }

        if (eng.inputIsMouseButtonDown(Input::MouseButton::Right)) {
            glm::vec3 hitPoint = eng.raycastFromMouse(100.0f);
            std::cout << "Raycast hit: (" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")" << std::endl;
        }

        eng.RenderScene();

        eng.windowEndFrame();
    }

    return 0;
}
