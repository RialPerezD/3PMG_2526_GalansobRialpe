#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>

#include <memory>

#include <MotArda/common/Systems/TraslationSystem.hpp>
#include <MotArda/common/Systems/RenderSystem.hpp>

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

    bool firstTime = true;
    MTRD::TransformComponent* t;
    MTRD::RenderComponent* r;
    MTRD::MovementComponent* m;
    glm::mat4x4 vp = glm::mat4(1.0f);
    glm::mat4x4 model = glm::mat4(1.0f);

    // --- Create drawable entitys ---
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t player = ecs.AddEntity();

    t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, 0, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.2f);

    r = ecs.AddComponent<MTRD::RenderComponent>(player);

    m = ecs.AddComponent<MTRD::MovementComponent>(player);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;
    // --- *** ---

    // --- Render System ---
    RenderSystem renderSystem;
    TranslationSystem translationSystem;
    // --- *** ---

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = {
        "indoor_plant_02.obj"
    };

    std::atomic<bool> objsLoaded = false;
    std::vector<ObjItem> objItemList;
    // --- *** ---

    // async obj load
    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );
    // --- *** ---

    // --- Camera ---
    MTRD::Camera camera = MTRD::Camera::CreateCamera(eng.windowGetSizeRatio());
    float movSpeed = 0.05f;
    // --- *** ---

    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (!objsLoaded) {
            printf("Cargando mayas...\n");
            eng.windowEndFrame();
            continue;

        }
        else if (firstTime) {
            firstTime = false;
            printf("Mayas cargadas correctamente\n");

            objItemList.push_back(std::move(eng.generateCube(5, 4)));

            // --- Load object materials ---
            // This needs be called here cuz uses gl calls and need object loaded
            eng.windowLoadAllMaterials(objItemList);
            // --- *** ---

            // --- Asign objects to renders ---
            r = ecs.GetComponent<MTRD::RenderComponent>(player);
            r->meshes_ = &objItemList[1].meshes;
            r->materials_ = &objItemList[0].materials;
            // --- *** ---
        }


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

        // --- update model matrix ---
        translationSystem.TranslationSystemWithMovementComponent(ecs, eng, renderSystem.uniforms, model);
        renderSystem.vp = vp;
        renderSystem.model = model;
        // --- *** ---

        renderSystem.Render(
            ecs,
            ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>(),
            true
        );
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}
