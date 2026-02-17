#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>

#include <memory>

#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"
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

    float frameTime = 0;
    bool firstTime = true;
    MTRD::TransformComponent* t;
    MTRD::RenderComponent* r;
    MTRD::MovementComponent* m;
    glm::mat4x4 vp, model;

    // --- Setup uniforms ---
    std::vector<Window::UniformAttrib> uniforms = {
        {"VP", -1, Window::UniformTypes::Mat4, glm::value_ptr(vp)},
        {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "position", 3, offsetof(Vertex, position) },
        { "uv", 2, offsetof(Vertex, uv) },
        { "normal", 3, offsetof(Vertex, normal) }
    };
    // --- *** ---

    // --- Create drawable entitys ---
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();

    size_t player = ecs.AddEntity();

    t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0);
    t->rotation = glm::vec3(1, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.02f);

    r = ecs.AddComponent<MTRD::RenderComponent>(player);

    m = ecs.AddComponent<MTRD::MovementComponent>(player);
    m->position = glm::vec3(0);
    m->rotation = glm::vec3(0, 0, 1);
    m->scale = glm::vec3(0.0f);
    m->shouldConstantMove = false;
    // --- *** ---
    
    // --- Render System ---
    RenderSystem renderSystem;
    // --- *** ---

    // --- Load Objs ---
    std::vector <const char*> objsRoutes = {
        "12140_Skull_v3_L2.obj"
    };

    std::atomic<bool> objsLoaded = false;
    std::vector<MTRD::Window::ObjItem> objItemList;
    // --- *** ---

    // async obj load
    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );
    // --- *** ---

    // --- Drawable transform additions ---
    float ratio = eng.windowGetSizeRatio();
    float movSpeed = 0.05f;
    // --- *** ---

    // --- Camera ---
    MTRD::Camera camera(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::radians(45.f),
        ratio,
        0.1f,
        100.f
    );

    camera.updateAll();
    // --- *** ---
    
    // --- Main window bucle ---
    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        if (!objsLoaded) {
            printf("Cargando mayas...\n");
            eng.windowEndFrame();
            continue;

        } else if (firstTime) {
            firstTime = false;

            // --- Load object materials ---
            // This needs be called here cuz uses gl calls and need object loaded
            eng.windowLoadAllMaterials(objItemList);
            // --- *** ---

            // --- Asign objects to renders ---
            r = ecs.GetComponent<MTRD::RenderComponent>(player);
            r->meshes_ = &objItemList[0].meshes;
            r->materials_ = &objItemList[0].materials;
            // --- *** ---
        }


        // --- update vp ---
        vp = camera.getViewProj();
        // --- *** ---


        // --- Setup uniforms and draw ---
        //TranslationSystem::TranslationSystemWithMovementComponent(ecs, eng, uniforms, model);
        
        renderSystem.Render(
            ecs,
            ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>(),
            true
        );
        // --- *** ---

        eng.windowEndFrame();

        frameTime = eng.windowGetLastFrameTime();
        //printf("Last frame time: %.4f secs\n", frameTime);
    }

    return 0;
}
