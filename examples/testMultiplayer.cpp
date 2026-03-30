#include "MotArda/common/Engine.hpp"
#include "MotArda/common/NetworkManager.hpp"
#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"

int MTRD::main() {
    constexpr bool IS_SERVER = true;
    constexpr uint16_t PORT = 1234;
    constexpr const char* SERVER_IP = "127.0.0.1";

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda Multiplayer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    eng.SetDebugMode(true);

    MTRD::NetworkManager netMgr;
    ECSManager& ecs = eng.getEcs();

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

    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();

    size_t entity1 = ecs.AddEntity();
    auto* netComp1 = ecs.AddComponent<MTRD::NetworkComponent>(entity1);
    netComp1->networkID = 1;
    netComp1->isLocal = true;

    auto* transform1 = ecs.AddComponent<MTRD::TransformComponent>(entity1);
    transform1->position = glm::vec3(0.0f, 0.0f, 0.0f);

    printf("Entity created with networkID=%u, isLocal=%s\n",
        netComp1->networkID,
        netComp1->isLocal ? "true" : "false");

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        MTRD::NetworkSystem::Process(ecs, eng, netMgr);

        eng.windowEndFrame();
    }

    netMgr.Shutdown();
    return 0;
}
