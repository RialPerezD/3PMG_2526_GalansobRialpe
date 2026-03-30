#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"

void MTRD::NetworkSystem::Process(ECSManager& ecs, MotardaEng& eng, NetworkManager& netMgr, std::function<void(uint32_t, const void*, size_t)> receiveCallback) {
    if (!netMgr.IsConnected()) return;

    netMgr.PollEvents(receiveCallback);

    auto entities = ecs.GetEntitiesWithComponents<TransformComponent, NetworkComponent>();

    if (netMgr.IsServer()) {
        for (size_t entity : entities) {
            auto* netComp = ecs.GetComponent<NetworkComponent>(entity);
            auto* transform = ecs.GetComponent<TransformComponent>(entity);

            if (netComp && transform && netComp->isLocal) {
                NetworkMessage msg;
                msg.networkID = netComp->networkID;
                msg.posX = transform->position.x;
                msg.posY = transform->position.y;
                msg.posZ = transform->position.z;
                msg.rotX = transform->rotation.x;
                msg.rotY = transform->rotation.y;
                msg.rotZ = transform->rotation.z;

                netMgr.BroadcastPacket(&msg, sizeof(msg), false);
            }
        }
    } else {
        for (size_t entity : entities) {
            auto* netComp = ecs.GetComponent<NetworkComponent>(entity);
            auto* transform = ecs.GetComponent<TransformComponent>(entity);

            if (netComp && transform && netComp->isLocal && netComp->networkID != 0) {
                NetworkMessage msg;
                msg.networkID = netComp->networkID;
                msg.posX = transform->position.x;
                msg.posY = transform->position.y;
                msg.posZ = transform->position.z;
                msg.rotX = transform->rotation.x;
                msg.rotY = transform->rotation.y;
                msg.rotZ = transform->rotation.z;

                netMgr.SendPacket(0, &msg, sizeof(msg), false);
            }
        }
    }
}

void MTRD::NetworkSystem::HandleReceive(uint32_t senderID, const void* data, size_t size) {
    if (size < sizeof(NetworkMessage)) return;

    const NetworkMessage* msg = static_cast<const NetworkMessage*>(data);
    (void)senderID;
    (void)msg;
}
