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

size_t lightEntity;

void GeneratePointLightEntitys(ECSManager& ecs, std::vector<MTRD::ObjItem>& objItemList) {
    size_t player = ecs.AddEntity();
    size_t floor = ecs.AddEntity();
    size_t cubes[4] = { ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity(), ecs.AddEntity() };
    lightEntity = ecs.AddEntity();

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
}


void GeneratePointLights(MTRD::LightComponent* lightComp, MTRD::MotardaEng& eng) {
    lightComp->pointLights.push_back(
        MTRD::PointLight(
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            1.0f,
            1.0f,
            0.09f,
            0.032f
        )
    );
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


    // --- Camera ---
    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 1, 20));
    camera.setTarget(glm::vec3(0.0f, -5.0f, 0.0f));
    float movSpeed = 0.1f;
    // --- *** ---


    // --- Setup engine info ---
    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::LightsWithShadows);
    eng.windowSetErrorCallback(error_callback);
    // --- *** ---


    // --- Create Geometry to use in elements ---
    std::vector<ObjItem> objItemList;
    objItemList.push_back(std::move(eng.generateSphere(0.5f, 20, 20, 1)));
    objItemList.push_back(std::move(eng.generatePlane(20, 20, 1)));
    objItemList.push_back(std::move(eng.generateCube(1)));
    eng.windowLoadAllMaterials(objItemList);
    // --- *** ---


    // --- Create drawable entitys ---
    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();
    ecs.AddComponentType<MTRD::LightComponent>();

    GeneratePointLightEntitys(ecs, objItemList);
    // --- *** ---


    // --- Lights ---
    // hacer que la maya tenga un booleano para si hace hace sombras o no
    MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(lightEntity);
    GeneratePointLights(lightComp, eng);
    // --- *** ---


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


        ImGui::SetNextWindowSize(ImVec2(250, 350), ImGuiCond_FirstUseEver);
        ImGui::Begin("Light Config");
        if (lightComp && !lightComp->pointLights.empty()) {
            MTRD::PointLight& light = lightComp->pointLights[0];
            ImGui::DragFloat3("Position", &light.position_[0], 0.1f);
            ImGui::ColorEdit3("Color", &light.color_[0]);
            ImGui::SliderFloat("Constant", &light.constant_, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &light.linear_, 0.0f, 0.5f);
            ImGui::SliderFloat("Quadratic", &light.quadratic_, 0.0f, 0.1f);
        }
        ImGui::End();

        // Generate shadow map
        eng.RenderScene();
        // --- *** ---

        eng.windowEndFrame();
    }

    return 0;
}