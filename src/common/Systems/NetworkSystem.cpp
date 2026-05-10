#include "MotArda/common/Systems/NetworkSystem.hpp"
#include "MotArda/common/Components/NetworkComponent.hpp"
#include "MotArda/common/Components/TransformComponent.hpp"
#include <cstring>
#include <iostream>
#include <vector>

namespace MTRD {

    NetworkSystem::NetworkSystem(ECSManager& ecs,
        NetworkManager& netMgr,
        MessageCallback receiveCallback)
        : ecs_(ecs),
        netMgr_(netMgr),
        receiveCallback_(receiveCallback),
        chatCallback_(nullptr) {
    }

    void NetworkSystem::Process() {
        if (!netMgr_.IsConnected()) return;

        netMgr_.PollEvents([this](uint32_t senderID, const void* data, size_t size) {
            HandleReceive(senderID, data, size);
            });

        auto entities = ecs_.GetEntitiesWithComponents<TransformComponent, NetworkComponent>();

        for (size_t entity : entities) {
            auto* netComp = ecs_.GetComponent<NetworkComponent>(entity);
            auto* transform = ecs_.GetComponent<TransformComponent>(entity);

            if (netComp && transform && (netMgr_.IsServer() || netComp->isLocal)) {
                if (!netMgr_.IsServer() && netComp->networkID == 0) continue;

                struct FullUpdatePacket {
                    NetMessage header;
                    EntityUpdatePayload payload;
                } packet;

                packet.header.type = MessageType::EntityUpdate;
                packet.header.senderId = netComp->networkID;

                packet.payload.networkID = netComp->networkID;
                packet.payload.meshId_ = netComp->meshId_;
                packet.payload.posX = transform->position.x;
                packet.payload.posY = transform->position.y;
                packet.payload.posZ = transform->position.z;
                packet.payload.rotX = transform->rotation.x;
                packet.payload.rotY = transform->rotation.y;
                packet.payload.rotZ = transform->rotation.z;

                if (netMgr_.IsServer()) {
                    netMgr_.BroadcastPacket(&packet, sizeof(packet), false);
                } else {
                    netMgr_.SendPacket(0, &packet, sizeof(packet), false);
                }
            }
        }
    }

    void NetworkSystem::HandleReceive(uint32_t senderID, const void* data, size_t size) {
        if (!data) return;

        if (receiveCallback_) {
            receiveCallback_(senderID, data, size);
        }

        if (size < sizeof(NetMessage)) return;

        const NetMessage* header = static_cast<const NetMessage*>(data);

        switch (header->type) {
        case MessageType::EntityUpdate: {
            if (size >= sizeof(NetMessage) + sizeof(EntityUpdatePayload)) {
                const EntityUpdatePayload* payload = reinterpret_cast<const EntityUpdatePayload*>(
                    static_cast<const uint8_t*>(data) + sizeof(NetMessage)
                    );

                auto entities = ecs_.GetEntitiesWithComponents<NetworkComponent, TransformComponent>();
                for (size_t entity : entities) {
                    auto* netComp = ecs_.GetComponent<NetworkComponent>(entity);
                    if (netComp && netComp->networkID == payload->networkID && !netComp->isLocal) {
                        auto* transform = ecs_.GetComponent<TransformComponent>(entity);
                        transform->position = { payload->posX, payload->posY, payload->posZ };
                        transform->rotation = { payload->rotX, payload->rotY, payload->rotZ };
                        break;
                    }
                }
            }
            break;
        }
        case MessageType::Chat: {
            if (chatCallback_ && size >= sizeof(NetMessage) + sizeof(ChatPayload)) {
                const ChatPayload* payload = reinterpret_cast<const ChatPayload*>(
                    static_cast<const uint8_t*>(data) + sizeof(NetMessage)
                    );
                chatCallback_(header->senderId, *payload);
            }
            break;
        }
        default:
            break;
        }
    }
}