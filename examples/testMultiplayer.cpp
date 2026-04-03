#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include "MotArda/common/Ecs.hpp"
#include "MotArda/common/NetworkManager.hpp"
#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"
#include "MotArda/common/Components/RenderComponent.hpp"
#include "MotArda/common/Components/MovementComponent.hpp"
#include "MotArda/common/SimplePacketReciver.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <functional>

int MTRD::main() {
    constexpr bool IS_SERVER = true;
    constexpr uint16_t PORT = 1234;
    constexpr const char* SERVER_IP = "127.0.0.1";      //This is loopback default ip
    //Change this to change the mesh of the player (0 for cube, 1 for sphere, 2 for sphere with texture)
	constexpr float meshIdSelector = 0;             

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MotardaEng::createEngine(800, 600, "Motarda Multiplayer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    eng.SetDebugMode(true);
    eng.SetRenderType(MotardaEng::RenderType::Base);

    Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 5, 10));
    camera.setTarget(glm::vec3(0, 0, 0));

    std::vector<ObjItem> objItemList;
    objItemList.push_back(eng.generateCube(1));
    objItemList.push_back(eng.generateSphere(0.5f, 20, 20));
    objItemList.push_back(eng.generateSphere(0.5f, 20, 20, 1));
    eng.windowLoadAllMaterials(objItemList);

    ECSManager& ecs = eng.getEcs();
    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();

    size_t player = SIZE_MAX;

    if (!IS_SERVER) {
        player = ecs.AddEntity();

        float randomX = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 6.0f)) - 3.0f;
        float randomZ = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 6.0f)) - 3.0f;

        auto* netComp = ecs.AddComponent<MTRD::NetworkComponent>(player);
        netComp->networkID = 0;
		netComp->meshId_ = meshIdSelector;
        netComp->isLocal = true;

        auto* transform = ecs.AddComponent<MTRD::TransformComponent>(player);
        transform->position = glm::vec3(randomX, 0, randomZ);
        transform->rotation = glm::vec3(0, 0, 0);
        transform->angleRotationRadians = 0;
        transform->scale = glm::vec3(1.f);

        auto* render = ecs.AddComponent<MTRD::RenderComponent>(player);
		assert(meshIdSelector < objItemList.size());
        render->meshes_ = &objItemList[meshIdSelector].meshes;
        render->materials_ = &objItemList[meshIdSelector].materials;

        printf("Player created at (%.2f, %.2f), waiting for networkID...\n",
            randomX, randomZ);
    }

    // --- Network management ---
    SimplePacketReciver simplPacRec 
        = SimplePacketReciver(
            &objItemList,
            &ecs,
            player);

    NetworkManager netMgr;
    if (IS_SERVER) {
        if (!netMgr.InitServer(PORT, 32)) {
            printf("Failed to start server\n");
            return 1;
        }
        printf("=== SERVER MODE ===\n");
    } else {
        if (!netMgr.InitClient(SERVER_IP, PORT)) {
            printf("Failed to connect to server\n");
            return 1;
        }
        printf("=== CLIENT MODE ===\n");
    }

    NetworkSystem netSys = NetworkSystem(
        ecs,
        netMgr,
        std::bind(
            &MTRD::SimplePacketReciver::OnReceivePacket,
            &simplPacRec,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));
    // --- *** ---

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (!IS_SERVER && player != SIZE_MAX) {
            auto* transform = ecs.GetComponent<MTRD::TransformComponent>(player);
            if (transform) {
                if (eng.inputIsKeyPressed(Input::Keyboard::W))
                    transform->position.z -= 0.1f;
                if (eng.inputIsKeyPressed(Input::Keyboard::S))
                    transform->position.z += 0.1f;
                if (eng.inputIsKeyPressed(Input::Keyboard::A))
                    transform->position.x -= 0.1f;
                if (eng.inputIsKeyPressed(Input::Keyboard::D))
                    transform->position.x += 0.1f;
            }
        }

        netSys.Process();

        eng.RenderScene();

        eng.windowEndFrame();
    }

    netMgr.Shutdown();
    return 0;
}
