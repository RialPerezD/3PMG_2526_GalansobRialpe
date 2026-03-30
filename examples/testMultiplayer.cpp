#include "MotArda/common/Engine.hpp"
#include "MotArda/common/ObjLoader.hpp"
#include "MotArda/common/Ecs.hpp"
#include "MotArda/common/NetworkManager.hpp"
#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"
#include "MotArda/common/Components/RenderComponent.hpp"
#include "MotArda/common/Components/MovementComponent.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <functional>

static std::map<uint32_t, size_t> remoteEntities;
static std::vector<MTRD::ObjItem>* sphereObjItem = nullptr;
static ECSManager* ecsPtr = nullptr;
static std::vector<MTRD::ObjItem>* objItemListPtr = nullptr;
static size_t localPlayerEntity = SIZE_MAX;

void OnReceivePacket(uint32_t senderID, const void* data, size_t size) {
    if (!ecsPtr) return;

    if (localPlayerEntity != SIZE_MAX) {
        auto* localNetComp = ecsPtr->GetComponent<MTRD::NetworkComponent>(localPlayerEntity);
        if (localNetComp && localNetComp->networkID == 0 && size == sizeof(uint32_t)) {
            uint32_t assignedID = *(uint32_t*)data;
            localNetComp->networkID = assignedID;
            printf("Assigned networkID: %u\n", assignedID);
            return;
        }
    }

    if (!objItemListPtr) return;

    if (senderID & 0x80000000) {
        uint32_t disconnectedID = senderID & 0x7FFFFFFF;
        auto it = remoteEntities.find(disconnectedID);
        if (it != remoteEntities.end()) {
            ecsPtr->RemoveEntity(it->second);
            remoteEntities.erase(it);
            printf("Removed entity for disconnected client %u\n", disconnectedID);
        }
        return;
    }

    if (size == 0 && senderID != 0) {
        printf("New client connected with ID %u\n", senderID);
        return;
    }

    if (size < sizeof(MTRD::NetworkMessage)) return;

    const MTRD::NetworkMessage* msg = static_cast<const MTRD::NetworkMessage*>(data);

    if (localPlayerEntity != SIZE_MAX) {
        auto* localNetComp = ecsPtr->GetComponent<MTRD::NetworkComponent>(localPlayerEntity);
        if (localNetComp && localNetComp->networkID == msg->networkID) {
            return;
        }
    }

    auto it = remoteEntities.find(msg->networkID);
    if (it == remoteEntities.end()) {
        size_t entity = ecsPtr->AddEntity();

        auto* netComp = ecsPtr->AddComponent<MTRD::NetworkComponent>(entity);
        netComp->networkID = msg->networkID;
        netComp->isLocal = false;

        auto* transform = ecsPtr->AddComponent<MTRD::TransformComponent>(entity);
        transform->position = glm::vec3(msg->posX, msg->posY, msg->posZ);
        transform->rotation = glm::vec3(msg->rotX, msg->rotY, msg->rotZ);
        transform->angleRotationRadians = 0;
        transform->scale = glm::vec3(1.f);

        if (!objItemListPtr->empty()) {
            auto* render = ecsPtr->AddComponent<MTRD::RenderComponent>(entity);
            render->meshes_ = &(*objItemListPtr)[0].meshes;
            render->materials_ = &(*objItemListPtr)[0].materials;
        }

        remoteEntities[msg->networkID] = entity;
        printf("Created remote entity for client %u\n", msg->networkID);
    } else {
        size_t entity = it->second;
        auto* transform = ecsPtr->GetComponent<MTRD::TransformComponent>(entity);
        if (transform) {
            transform->position = glm::vec3(msg->posX, msg->posY, msg->posZ);
            transform->rotation = glm::vec3(msg->rotX, msg->rotY, msg->rotZ);
        }
    }
}

int MTRD::main() {
    constexpr bool IS_SERVER = true;
    constexpr uint16_t PORT = 1234;
    constexpr const char* SERVER_IP = "127.0.0.1";

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda Multiplayer");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    eng.SetDebugMode(true);
    eng.SetRenderType(MTRD::MotardaEng::RenderType::Base);

    MTRD::Camera& camera = eng.getCamera();
    camera.setPosition(glm::vec3(0, 5, 10));
    camera.setTarget(glm::vec3(0, 0, 0));

    MTRD::NetworkManager netMgr;
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

    std::vector<ObjItem> objItemList;
    objItemList.push_back(eng.generateSphere(0.5f, 20, 20));
    eng.windowLoadAllMaterials(objItemList);
    sphereObjItem = &objItemList;
    objItemListPtr = &objItemList;

    ECSManager& ecs = eng.getEcs();
    ecsPtr = &ecs;
    ecs.AddComponentType<MTRD::NetworkComponent>();
    ecs.AddComponentType<MTRD::TransformComponent>();
    ecs.AddComponentType<MTRD::RenderComponent>();

    size_t player = SIZE_MAX;

    if (!IS_SERVER) {
        player = ecs.AddEntity();
        localPlayerEntity = player;

        float randomX = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 6.0f)) - 3.0f;
        float randomZ = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 6.0f)) - 3.0f;

        auto* netComp = ecs.AddComponent<MTRD::NetworkComponent>(player);
        netComp->networkID = 0;
        netComp->isLocal = true;

        auto* transform = ecs.AddComponent<MTRD::TransformComponent>(player);
        transform->position = glm::vec3(randomX, 0, randomZ);
        transform->rotation = glm::vec3(0, 0, 0);
        transform->angleRotationRadians = 0;
        transform->scale = glm::vec3(1.f);

        auto* render = ecs.AddComponent<MTRD::RenderComponent>(player);
        render->meshes_ = &objItemList[0].meshes;
        render->materials_ = &objItemList[0].materials;

        printf("Player created at (%.2f, %.2f), waiting for networkID...\n",
            randomX, randomZ);
    }

    float moveSpeed = 0.1f;

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        if (!IS_SERVER && player != SIZE_MAX) {
            auto* transform = ecs.GetComponent<MTRD::TransformComponent>(player);
            if (transform) {
                if (eng.inputIsKeyPressed(Input::Keyboard::W))
                    transform->position.z -= moveSpeed;
                if (eng.inputIsKeyPressed(Input::Keyboard::S))
                    transform->position.z += moveSpeed;
                if (eng.inputIsKeyPressed(Input::Keyboard::A))
                    transform->position.x -= moveSpeed;
                if (eng.inputIsKeyPressed(Input::Keyboard::D))
                    transform->position.x += moveSpeed;
            }
        }

        MTRD::NetworkSystem::Process(ecs, eng, netMgr, OnReceivePacket);

        eng.RenderScene();

        eng.windowEndFrame();
    }

    netMgr.Shutdown();
    return 0;
}
