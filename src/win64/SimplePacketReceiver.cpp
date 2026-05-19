#include <MotArda/SimplePacketReceiver.hpp>
#include <MotArda/Systems/NetworkSystem.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/Components/NetworkComponent.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>

namespace MTRD {

    SimplePacketReceiver::SimplePacketReceiver()
        : ecsPtr(nullptr),
        objItemListPtr(nullptr),
        localPlayerEntity(SIZE_MAX) {
    }

    SimplePacketReceiver::SimplePacketReceiver(
        std::vector<std::shared_ptr<MTRD::ObjItem>>* objItemListPtr,
        ECSManager* ecsPtr,
        size_t localPlayerEntity
    ) {
        this->objItemListPtr = objItemListPtr;
        this->ecsPtr = ecsPtr;
        this->localPlayerEntity = localPlayerEntity;
    }

    void SimplePacketReceiver::OnReceivePacket(uint32_t senderID, const void* data, size_t size) {
        if (!ecsPtr || !data) return;

        // 1. Manejar asignación de ID local (paquetes crudos del NetworkManager de 4 bytes)
        if (localPlayerEntity != SIZE_MAX && size == sizeof(uint32_t)) {
            auto* localNetComp = ecsPtr->GetComponent<MTRD::NetworkComponent>(localPlayerEntity);
            if (localNetComp && localNetComp->networkID == 0) {
                uint32_t assignedID = *static_cast<const uint32_t*>(data);
                localNetComp->networkID = assignedID;
                MTRD::Logger::info("Assigned local networkID: {}\n", assignedID);
                return;
            }
        }

        // 2. Manejar desconexiones (Flag MSB del NetworkManager)
        if (senderID & 0x80000000) {
            uint32_t disconnectedID = senderID & 0x7FFFFFFF;
            auto it = remoteEntities.find(disconnectedID);
            if (it != remoteEntities.end()) {
                ecsPtr->RemoveEntity(it->second);
                remoteEntities.erase(it);
                MTRD::Logger::info("Removed entity for disconnected client {}\n", disconnectedID);
            }
            return;
        }

        // 3. Procesar mensajes con protocolo NetMessage (Header + Payload)
        if (size < sizeof(NetMessage)) return;

        const NetMessage* header = static_cast<const NetMessage*>(data);
        const uint8_t* payloadPtr = static_cast<const uint8_t*>(data) + sizeof(NetMessage);

        switch (header->type) {
        case MessageType::EntityUpdate: {
            if (size < sizeof(NetMessage) + sizeof(EntityUpdatePayload)) return;

            const EntityUpdatePayload* payload = reinterpret_cast<const EntityUpdatePayload*>(payloadPtr);

            // Ignorar si es nuestro propio ID (evitar eco)
            if (localPlayerEntity != SIZE_MAX) {
                auto* localNetComp = ecsPtr->GetComponent<MTRD::NetworkComponent>(localPlayerEntity);
                if (localNetComp && localNetComp->networkID == payload->networkID) return;
            }

            auto it = remoteEntities.find(payload->networkID);
            if (it == remoteEntities.end()) {
                // Crear nueva entidad remota
                size_t entity = ecsPtr->AddEntity();

                auto* netComp = ecsPtr->AddComponent<MTRD::NetworkComponent>(entity);
                netComp->networkID = payload->networkID;
                netComp->meshId_ = payload->meshId_;
                netComp->isLocal = false;

                auto* transform = ecsPtr->AddComponent<MTRD::TransformComponent>(entity);
                transform->position = { payload->posX, payload->posY, payload->posZ };
                transform->rotation = { payload->rotX, payload->rotY, payload->rotZ };
                transform->scale = glm::vec3(1.0f);

                if (objItemListPtr && !objItemListPtr->empty()) {
                    auto* render = ecsPtr->AddComponent<MTRD::RenderComponent>(entity);
                    size_t meshIdx = static_cast<size_t>(payload->meshId_);
                    if (meshIdx < objItemListPtr->size()) {
                        render->objitem_ = (*objItemListPtr)[meshIdx];
                    }
                }

                remoteEntities[payload->networkID] = entity;
                MTRD::Logger::info("Created remote entity for client {}\n", payload->networkID);
            } else {
                // Actualizar entidad existente
                size_t entity = it->second;
                auto* transform = ecsPtr->GetComponent<MTRD::TransformComponent>(entity);
                if (transform) {
                    transform->position = { payload->posX, payload->posY, payload->posZ };
                    transform->rotation = { payload->rotX, payload->rotY, payload->rotZ };
                }
            }
            break;
        }

        case MessageType::Chat: {
            if (size < sizeof(NetMessage) + sizeof(ChatPayload)) return;
            const ChatPayload* payload = reinterpret_cast<const ChatPayload*>(payloadPtr);
            MTRD::Logger::info("Chat message from {}: {}\n", header->senderId, payload->text);
            break;
        }

        default:
            break;
        }
    }
}