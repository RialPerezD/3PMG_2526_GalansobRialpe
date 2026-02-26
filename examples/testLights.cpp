#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include <MotArda/common/Ecs.hpp>
#include <MotArda/common/Camera.hpp>
#include <MotArda/common/Components/LightComponent.hpp>

#include <memory>

#include <MotArda/common/Systems/TraslationSystem.hpp>
#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Systems/ShadowMapSystem.hpp>

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
    ecs.AddComponentType<MTRD::LightComponent>();

    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };
    size_t lightEntity = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, -2.f, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(1.f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->meshes_ = &objItemList[0].meshes;
    r->materials_ = &objItemList[0].materials;

    MTRD::MovementComponent* m = ecs.AddComponent<MTRD::MovementComponent>(player);
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
        t->position = glm::vec3(-5 * ((i % 2) * 2 - 1), -2.0f, -5 * ((i / 2) * 2 - 1));
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
    camera.setTarget(glm::vec3(0.0f, -5.0f, 0.0f));
    glm::vec3 viewPos = camera.getPosition();
    float movSpeed = 0.05f;

    glm::mat4x4 vp = glm::mat4(1.0f);
    glm::mat4x4 model = glm::mat4(1.0f);
    // --- *** ---


    // --- Render System ---
    RenderLightsSystem renderLightsSystem = RenderLightsSystem(vp, model, viewPos);
    ShadowMapSystem shadowSystem = ShadowMapSystem(model);
    TranslationSystem translationSystem;
    // --- *** ---


    // --- Lights ---
    // hacer que la maya tenga un booleano para si hace hace sombras o no
    MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(lightEntity);

    lightComp->spotLights.push_back(
        SpotLight(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            7.0f,
            1.f,
            0.95f,
            1.0f,
            0.09f,
            0.032f,
            eng.windowGetSizeRatio()
        )
    );

    lightComp->spotLights.push_back(
        SpotLight(
            glm::vec3(4.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            7.0f,
            1.0f,
            0.95f,
            1.0f,
            0.09f,
            0.032f,
            eng.windowGetSizeRatio()
        )
    );

    /*lightComp->spotLights.push_back(
        SpotLight(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            8.0f,
            1.0f,
            0.85f,
            1.0f,
            0.09f,
            0.032f,
            eng.windowGetSizeRatio()
        )
    );*/
    // --- *** ---

    float angulo = 0.f;
    float radio = 10.f;
    float velocidad = 1.f;
    float M_PI = 3.14159265359f;
    float timer = 0.0f;

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

        // --- update vp ---
        vp = camera.getViewProj();
        viewPos = camera.getPosition();
        // --- *** ---

        timer += eng.windowGetLastFrameTime();

        float posX = radio * cos(timer * velocidad);
        float posY = radio * sin(timer * velocidad);
        
		/*
        lightComp->spotLights[0].position_ = glm::vec3(posX, 5.0f, posY);
		lightComp->spotLights[0].direction_ = glm::normalize(glm::vec3(0,-3,0) - lightComp->spotLights[0].position_);

        lightComp->spotLights[1].position_ = glm::vec3(-posX, 5.0f, -posY);
        lightComp->spotLights[1].direction_ = glm::normalize(glm::vec3(0, -3, 0) - lightComp->spotLights[1].position_);
        */
        
        //lightComp->spotLights[2].position_ = glm::vec3(posX * 0.75f, 2.0f, -posY * 0.75f);
        

        // Generate shadow map
        shadowSystem.RenderShadowMap(ecs, model);
        renderLightsSystem.SetShadowMaps(shadowSystem.getAllDepthMaps());

        //Now normal Render
        renderLightsSystem.Render(ecs, model, true);
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}