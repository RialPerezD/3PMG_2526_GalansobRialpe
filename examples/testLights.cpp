#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>

#include <memory>

#include <MotArda/common/Systems/TraslationSystem.hpp>
#include <MotArda/win64/Systems/RenderSystem.hpp>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}


int MTRD::main() {
    // --- Rand seed ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // --- *** ---


    // --- Create engine ---
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();
    // --- *** ---


    // --- Setup callback inside window ---
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---


    // --- Ecs ---
    ECSManager ecs;
    // --- *** ---


    // --- Create Geometry to use in elements ---
    std::vector<ObjItem> objItemList;
    objItemList.push_back(std::move(eng.generateSphere(0.5f, 20, 20)));
    objItemList.push_back(std::move(eng.generatePlane(20, 20)));
    objItemList.push_back(std::move(eng.generateCube(1)));
    eng.windowLoadAllMaterials(objItemList);
    // --- *** ---


    // --- Create drawable entitys ---
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };

    MTRD::TransformComponent*  t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, -2.5f, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent*  r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->meshes_ = &objItemList[0].meshes;
    r->materials_ = &objItemList[0].materials;

    MTRD::MovementComponent*  m = ecs.AddComponent<MTRD::MovementComponent>(player);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;



    t = ecs.AddComponent<MTRD::TransformComponent>(floor);
    t->position = glm::vec3(0, -3, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    r = ecs.AddComponent<MTRD::RenderComponent>(floor);
    r->meshes_ = &objItemList[1].meshes;
    r->materials_ = &objItemList[1].materials;

    m = ecs.AddComponent<MTRD::MovementComponent>(floor);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;


    for (int i = 0; i < 4; i++) {
        t = ecs.AddComponent<MTRD::TransformComponent>(cubes[i]);
        t->position = glm::vec3(-5 * ((i % 2) * 2 - 1), -2.5f, -5 * ((i / 2) * 2 - 1));
        t->rotation = glm::vec3(0, 0, 0);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(1.f);

        r = ecs.AddComponent<MTRD::RenderComponent>(cubes[i]);
        r->meshes_ = &objItemList[2].meshes;
        r->materials_ = &objItemList[2].materials;

        m = ecs.AddComponent<MTRD::MovementComponent>(cubes[i]);
        m->position = glm::vec3(0);
        m->rotation = glm::vec3(0, 0, 1);
        m->scale = glm::vec3(0.0f);
        m->shouldConstantMove = false;
    }
    // --- *** ---


    // --- Camera ---
    MTRD::Camera camera = MTRD::Camera::CreateCamera(eng.windowGetSizeRatio());
    camera.setPosition(glm::vec3(0, 1, 20));
    float movSpeed = 0.05f;

    glm::mat4x4 vp = glm::mat4(1.0f);
    glm::mat4x4 model = glm::mat4(1.0f);
    // --- *** ---


    // --- Render System ---
    RenderSystem renderSystem = RenderSystem(vp, model);
    TranslationSystem translationSystem;
    // --- *** ---
    

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        // --- Input to move camera ---
        if (eng.inputIsKeyPressed(Input::Keyboard::W)) camera.moveForward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::S)) camera.moveBackward(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::A)) camera.moveLeft(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) camera.moveRight(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::E)) camera.moveUp(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::Q)) camera.moveDown(movSpeed);
        if (eng.inputIsKeyPressed(Input::Keyboard::R)) camera.rotate(10.0f, 0.0f);
        if (eng.inputIsKeyPressed(Input::Keyboard::T)) camera.rotate(-10.0f, 0.0f);
        // --- *** ---

        // --- update vp ---
        vp = camera.getViewProj();
        // --- *** ---

        renderSystem.Render(
            ecs,
            ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>(),
            model,
            true
        );
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}
