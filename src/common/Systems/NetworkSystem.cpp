#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"

namespace MTRD {

    NetworkSystem::NetworkSystem(ECSManager& ecs,
        NetworkManager& netMgr,
        MessageCallback receiveCallback)
        : ecs_(ecs),
        netMgr_(netMgr),
        receiveCallback_(receiveCallback) 
    {}

    void NetworkSystem::Process() {
        if (!netMgr_.IsConnected()) return;

        netMgr_.PollEvents(receiveCallback_);

        auto entities = ecs_.GetEntitiesWithComponents<TransformComponent, NetworkComponent>();

        if (netMgr_.IsServer()) {
            for (size_t entity : entities) {
                auto* netComp = ecs_.GetComponent<NetworkComponent>(entity);
                auto* transform = ecs_.GetComponent<TransformComponent>(entity);

                if (netComp && transform && netComp->isLocal) {
                    NetworkMessage msg;
                    msg.networkID = netComp->networkID;
                    msg.meshId_ = netComp->meshId_;
                    msg.posX = transform->position.x;
                    msg.posY = transform->position.y;
                    msg.posZ = transform->position.z;
                    msg.rotX = transform->rotation.x;
                    msg.rotY = transform->rotation.y;
                    msg.rotZ = transform->rotation.z;

                    netMgr_.BroadcastPacket(&msg, sizeof(msg), false);
                }
            }
        } else {
            for (size_t entity : entities) {
                auto* netComp = ecs_.GetComponent<NetworkComponent>(entity);
                auto* transform = ecs_.GetComponent<TransformComponent>(entity);

                if (netComp && transform && netComp->isLocal && netComp->networkID != 0) {
                    NetworkMessage msg;
                    msg.networkID = netComp->networkID;
                    msg.meshId_ = netComp->meshId_;
                    msg.posX = transform->position.x;
                    msg.posY = transform->position.y;
                    msg.posZ = transform->position.z;
                    msg.rotX = transform->rotation.x;
                    msg.rotY = transform->rotation.y;
                    msg.rotZ = transform->rotation.z;

                    netMgr_.SendPacket(0, &msg, sizeof(msg), false);
                }
            }
        }
    }

    void NetworkSystem::HandleReceive(uint32_t senderID, const void* data, size_t size) {
        if (size < sizeof(NetworkMessage)) return;

        const NetworkMessage* msg = static_cast<const NetworkMessage*>(data);
        (void)senderID;
        (void)msg;
    }
}
