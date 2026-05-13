#include <MotArda/Engine.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Components/LightComponent.hpp>
#include <MotArda/Systems/RenderLightsSystem.hpp>
#include <MotArda/Systems/ShadowMapSystem.hpp>
#include <MotArda/Terrain.hpp>

#include <memory>

static void error_callback(int error, const char* description) {
    MTRD::Logger::error("Glfw error: {}\n", description);

}

int MTRD::main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 50, 30));
    float movSpeed = 0.1f;
    bool followPlayer = false;
    bool pPressed = false;

    float orbitAngle = 0.0f;
    float orbitDistance = 15.0f;

    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::LightsWithShadows);
    eng.windowSetErrorCallback(error_callback);

    std::unique_ptr<MTRD::Terrain> terrain = eng.CreateTerrain(100, 100, 40);

    std::vector<std::shared_ptr<ObjItem>> objItemList;
    objItemList.push_back(eng.generateSphere(0.5f, 100, 100, 0));
    objItemList.push_back(terrain->ObjItem_);
    eng.windowLoadAllMaterials(objItemList);

    ECSManager& ecs = eng.getEcs();

    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::MovementComponent>();
    ecs.AddComponentType<MTRD::LightComponent>();

    size_t lightEntity = ecs.AddEntity();
    MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(lightEntity);
    lightComp->directionalLights.push_back(
        MTRD::DirectionalLight(
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.0f
        )
    );

    size_t player = ecs.AddEntity();

    MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(player);
    t->position = glm::vec3(0, 0, 0);
    t->rotation = glm::vec3(0, 0, 0);
    t->angleRotationRadians = -1;
    t->scale = glm::vec3(0.5f);

    MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(player);
    r->objitem_ = objItemList[0];

    size_t terrainEntity = ecs.AddEntity();

    MTRD::TransformComponent* tr = ecs.AddComponent<MTRD::TransformComponent>(terrainEntity);
    tr->position = glm::vec3(0, 0, 0);
    tr->rotation = glm::vec3(0, 0, 0);
    tr->angleRotationRadians = -1;
    tr->scale = glm::vec3(2.f);

    MTRD::RenderComponent* rr = ecs.AddComponent<MTRD::RenderComponent>(terrainEntity);
    rr->objitem_ = objItemList[1];

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (eng.inputIsKeyPressed(Input::Keyboard::P)) {
            if (!pPressed) {
                followPlayer = !followPlayer;
                pPressed = true;
            }
        } else {
            pPressed = false;
        }

        MTRD::TransformComponent* pt = ecs.GetComponent<MTRD::TransformComponent>(player);

        if (followPlayer) {
            glm::vec3 camFront = camera.getFront();
            glm::vec3 forward = glm::normalize(glm::vec3(camFront.x, 0.0f, camFront.z));
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

            if (eng.inputIsKeyPressed(Input::Keyboard::W)) pt->position += forward * movSpeed;
            if (eng.inputIsKeyPressed(Input::Keyboard::S)) pt->position -= forward * movSpeed;
            if (eng.inputIsKeyPressed(Input::Keyboard::A)) pt->position -= right * movSpeed;
            if (eng.inputIsKeyPressed(Input::Keyboard::D)) pt->position += right * movSpeed;

            if (eng.inputIsKeyPressed(Input::Keyboard::Q)) orbitAngle -= 0.05f;
            if (eng.inputIsKeyPressed(Input::Keyboard::E)) orbitAngle += 0.05f;

            float h = terrain->GetHeightAt(pt->position.x / tr->scale.x, pt->position.z / tr->scale.z);
            pt->position.y = (h * tr->scale.y) + 0.25f;

            float camX = pt->position.x + orbitDistance * sin(orbitAngle);
            float camZ = pt->position.z + orbitDistance * cos(orbitAngle);

            camera.setPosition(glm::vec3(camX, pt->position.y + 1.0f, camZ));
            camera.setTarget(pt->position);
        } else {
            if (eng.inputIsKeyPressed(Input::Keyboard::W)) camera.moveForward(movSpeed * 2.0f);
            if (eng.inputIsKeyPressed(Input::Keyboard::S)) camera.moveBackward(movSpeed * 2.0f);
            if (eng.inputIsKeyPressed(Input::Keyboard::A)) camera.moveLeft(movSpeed * 2.0f);
            if (eng.inputIsKeyPressed(Input::Keyboard::D)) camera.moveRight(movSpeed * 2.0f);

            if (eng.inputIsKeyPressed(Input::Keyboard::E)) camera.rotate(20.0f, 0.0f);
            if (eng.inputIsKeyPressed(Input::Keyboard::Q)) camera.rotate(-20.0f, 0.0f);

            if (eng.inputIsKeyPressed(Input::Keyboard::R)) camera.moveUp(movSpeed * 2.0f);
            if (eng.inputIsKeyPressed(Input::Keyboard::T)) camera.moveDown(movSpeed * 2.0f);

            if (eng.inputIsKeyPressed(Input::Keyboard::F)) camera.rotate(0.0f, 20.0f);
            if (eng.inputIsKeyPressed(Input::Keyboard::G)) camera.rotate(0.0f, -20.0f);
        }

        eng.RenderScene();
        eng.windowEndFrame();
    }

    return 0;
}