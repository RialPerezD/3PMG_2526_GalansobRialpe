#include <MotArda/Engine.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Components/LightComponent.hpp>
#include <MotArda/Components/PhysxComponent.hpp>

#include <memory>

static void error_callback(int error, const char* description) {
    MTRD::Logger::error("Glfw error: {}\n", description);

}


int MTRD::main() {
    // --- Rand seed ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // --- *** ---


    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();
    MTRD::Logger::init("testPhysx", MTRD::Logger::Level::Debug);


    // --- *** ---


    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 1, 20));
    camera.setTarget(glm::vec3(0.0f, -5.0f, 0.0f));
    float movSpeed = 0.1f;
    // --- *** ---


    // --- Setup engine info ---
	eng.SetDebugMode(true);
	eng.SetRenderType(MotardaEng::RenderType::Base);
    // Enable physx
	eng.hasPhysx(true);
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---


    // --- Create Geometry to use in elements ---
    std::vector<std::shared_ptr<ObjItem>> objItemList;
    objItemList.push_back(eng.generateSphere(0.5f, 20, 20));
    objItemList.push_back(eng.generatePlane(20, 20));
    objItemList.push_back(eng.generateCube(1));
    eng.windowLoadAllMaterials(objItemList);
    // --- *** ---


    // --- Create drawable entitys ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();
    ecs.AddComponentType<MTRD::LightComponent>();
    // New component to the ECS
    ecs.AddComponentType<MTRD::PhysxComponent>();

    // Create entities
    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t spheres[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, 5.f, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->objitem_ = objItemList[0];

    MTRD::MovementComponent* m = ecs.AddComponent<MTRD::MovementComponent>(player);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;

    MTRD::PhysxComponent* p = ecs.AddComponent<MTRD::PhysxComponent>(player);
    p->shapeType = MTRD::PhysxShapeType::Sphere;
    p->radius = 0.5f;
    p->mass = 1.0f;
    p->isDynamic = true;
    eng.createPhysxActor(*p, *t);


    t = ecs.AddComponent<MTRD::TransformComponent>(floor);
    t->position = glm::vec3(0, -3, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    r = ecs.AddComponent<MTRD::RenderComponent>(floor);
    r->objitem_ = objItemList[1];

    m = ecs.AddComponent<MTRD::MovementComponent>(floor);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;

    // Physx component for the floor
    MTRD::PhysxComponent* floorPhysx = ecs.AddComponent<MTRD::PhysxComponent>(floor);
    floorPhysx->shapeType = MTRD::PhysxShapeType::Box;
    floorPhysx->halfExtents = glm::vec3(10.0f, 0.05f, 10.0f);
    floorPhysx->mass = 0.0f;
    floorPhysx->isDynamic = false;
    eng.createPhysxActor(*floorPhysx, *t);

    
    for (int i = 0; i < 4; i++) {
        t = ecs.AddComponent<MTRD::TransformComponent>(spheres[i]);
        t->position = glm::vec3(0.5f * ((i % 2) * 2 - 1), -2.0f, 0.5f * ((i / 2) * 2 - 1));
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        r = ecs.AddComponent<MTRD::RenderComponent>(spheres[i]);
        r->objitem_ = objItemList[0];

        m = ecs.AddComponent<MTRD::MovementComponent>(spheres[i]);
        m->position = glm::vec3(0);
        m->rotation = glm::vec3(0, 0, 1);
        m->scale = glm::vec3(0.0f);
        m->shouldConstantMove = false;

        // Physx component for the spheres
        p = ecs.AddComponent<MTRD::PhysxComponent>(spheres[i]);
        p->shapeType = MTRD::PhysxShapeType::Sphere;
        p->radius = 0.5f;
        p->mass = 1.0f;
        p->isDynamic = true;
        eng.createPhysxActor(*p, *t);
    }
    // --- ***

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        // --- Input to move camera ---
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
        // --- *** ---

        // Generate shadow map
        MTRD::Logger::shutdown();
        eng.RenderScene();
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}
