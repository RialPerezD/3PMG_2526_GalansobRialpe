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

static void error_callback([[maybe_unused]] int error, const char* description) {
    MTRD::Logger::error("Glfw error: {}\n", description);

}

constexpr int NUM_TREES = 500;

int MTRD::main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 40, 60));
    camera.setTarget(glm::vec3(0, 0, 0));
    float movSpeed = 0.1f;
    bool pPressed = false;

    eng.setDebugMode(true);
    eng.setRenderType(MotardaEng::RenderType::LightsWithShadows);
    eng.windowSetErrorCallback(error_callback);

    std::unique_ptr<MTRD::Terrain> terrain = eng.createTerrain(100, 100, 40);

    MTRD::TreeGenerator::TreeProfile treeProfile;
    treeProfile.axiom = "X";
    treeProfile.rules = {
        {'X', "F[+XL][-X]FXL"},
        {'L', "L[+G][-G]G"}
    };
    treeProfile.iterations = 4;
    treeProfile.baseAngle = 25.0f;
    treeProfile.angleVariance = 15.0f;
    treeProfile.baseLength = 1.0f;
    treeProfile.lengthVariance = 0.2f;
    treeProfile.widthStep = 0.75f;
    treeProfile.initialWidth = 0.3f;
    treeProfile.leafChance = 0.85f;
    treeProfile.leafSize = 0.6f;

    std::vector<std::shared_ptr<ObjItem>> objItemList;
    objItemList.push_back(eng.generateSphere(0.5f, 100, 100, 0));
    objItemList.push_back(terrain->ObjItem_);
    for (int v = 0; v < NUM_TREES / 5; v++) {
        objItemList.push_back(eng.generateTree(treeProfile, 54321 + v, 0, false));
    }
    eng.windowLoadAllMaterials(objItemList);

    ECSManager& ecs = eng.getEcs();

    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();
    ecs.AddComponentType<MTRD::LightComponent>();

    size_t lightEntity = ecs.AddEntity();
    MTRD::LightComponent* lightComp = ecs.AddComponent<MTRD::LightComponent>(lightEntity);
    lightComp->hasAmbient_ = true;
    lightComp->ambient_ = MTRD::AmbientLight(glm::vec3(0.12f, 0.12f, 0.15f), 0.12f);
    lightComp->directionalLights.push_back(
        MTRD::DirectionalLight(
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.5f
        )
    );
    ecs.AddComponent<MTRD::TransformComponent>(lightEntity);

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

    glm::vec3 terrainScale = tr->scale;

    struct TreePos { float x; float z; };
    std::vector<TreePos> treePositions;
    treePositions.reserve(NUM_TREES);

    int maxAttempts = 10000;
    int treeCount = 0;
    for (int i = 0; i < NUM_TREES; i++) {
        bool found = false;
        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            float wx = (float)(std::rand() % 1801) / 10.0f - 90.0f;
            float wz = (float)(std::rand() % 1801) / 10.0f - 90.0f;

            bool valid = true;
            for (const auto& pos : treePositions) {
                float dx = wx - pos.x;
                float dz = wz - pos.z;
                if (dx * dx + dz * dz < 9.0f) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                treePositions.push_back({ wx, wz });

                float localX = wx / terrainScale.x;
                float localZ = wz / terrainScale.z;
                float h = terrain->GetHeightAt(localX, localZ);

                size_t treeEntity = ecs.AddEntity();
                MTRD::TransformComponent* tt = ecs.AddComponent<MTRD::TransformComponent>(treeEntity);
                tt->position = glm::vec3(wx, h * terrainScale.y, wz);
                tt->rotation = glm::vec3(0, 1, 0);
                tt->angleRotationRadians = (float)(std::rand() % 6283) / 1000.0f;
                tt->scale = glm::vec3(0.25f);
                MTRD::RenderComponent* rrt = ecs.AddComponent<MTRD::RenderComponent>(treeEntity);
                rrt->objitem_ = objItemList[2 + treeCount / 5];
                treeCount++;

                found = true;
                break;
            }
        }
        if (!found) {
            MTRD::Logger::warn("Could not place tree {} after {} attempts\n", i, maxAttempts);
        }
    }

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (eng.inputIsKeyPressed(Input::Keyboard::P)) {
            if (!pPressed && !treePositions.empty()) {
                int idx = std::rand() % (int)treePositions.size();
                float tx = treePositions[idx].x;
                float tz = treePositions[idx].z;
                float localX = tx / terrainScale.x;
                float localZ = tz / terrainScale.z;
                float h = terrain->GetHeightAt(localX, localZ) * terrainScale.y;
                camera.setPosition(glm::vec3(tx + 4.0f, h + 2.0f, tz + 4.0f));
                camera.setTarget(glm::vec3(tx, h + 1.0f, tz));
                pPressed = true;
            }
        } else {
            pPressed = false;
        }

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

        eng.renderScene();
        eng.windowEndFrame();
    }

    eng.endDebugger();

    return 0;
}