#include "MotArda/common/SimplePacketReciver.hpp"
#include <MotArda/common/Systems/NetworkSystem.hpp>

namespace MTRD {

    SimplePacketReciver::SimplePacketReciver()
        :ecsPtr(nullptr),
        objItemListPtr(nullptr),
        localPlayerEntity(SIZE_MAX) {
    }


    SimplePacketReciver::SimplePacketReciver(
        std::vector<MTRD::ObjItem>* objItemListPtr,
        ECSManager* ecsPtr,
        size_t localPlayerEntity
    ) {
        this->objItemListPtr = objItemListPtr;
        this->ecsPtr = ecsPtr;
		this->localPlayerEntity = localPlayerEntity;
    }


    void SimplePacketReciver::OnReceivePacket(uint32_t senderID, const void* data, size_t size) {
        if (!ecsPtr) return;

        // Handle local player network ID assignment
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

        // Check for disconnect flag (MSB set to 1)
        if (senderID & 0x80000000) {
            // Remove flag to get the actual client ID
            uint32_t disconnectedID = senderID & 0x7FFFFFFF;
            auto it = remoteEntities.find(disconnectedID);
            if (it != remoteEntities.end()) {
                ecsPtr->RemoveEntity(it->second);
                remoteEntities.erase(it);
                printf("Removed entity for disconnected client %u\n", disconnectedID);
            }
            return;
        }

        // New connection signal (empty packet)
        if (size == 0 && senderID != 0) {
            printf("New client connected with ID %u\n", senderID);
            return;
        }

        // Ignore packets that are too small
        if (size < sizeof(MTRD::NetworkMessage)) {
            return;
        }

        // Check if it's a chat message (exactly ChatMessage size)
        if (size == sizeof(MTRD::ChatMessage)) {
            const MTRD::ChatMessage* chatMsg = static_cast<const MTRD::ChatMessage*>(data);
            if (chatMsg->text[0] != '\0') {
                printf("Chat message received from %u: %s\n", senderID, chatMsg->text);
            }
            return;
        }

        const MTRD::NetworkMessage* msg = static_cast<const MTRD::NetworkMessage*>(data);

        // Ignore packets sent by the local player
        if (localPlayerEntity != SIZE_MAX) {
            auto* localNetComp = ecsPtr->GetComponent<MTRD::NetworkComponent>(localPlayerEntity);
            if (localNetComp && localNetComp->networkID == msg->networkID) {
                return;
            }
        }

        auto it = remoteEntities.find(msg->networkID);
        if (it == remoteEntities.end()) {
            // Create a new entity for a new discovered remote player
            size_t entity = ecsPtr->AddEntity();

            auto* netComp = ecsPtr->AddComponent<MTRD::NetworkComponent>(entity);
            netComp->networkID = msg->networkID;
			netComp->meshId_ = msg->meshId_;
            netComp->isLocal = false;

            auto* transform = ecsPtr->AddComponent<MTRD::TransformComponent>(entity);
            transform->position = glm::vec3(msg->posX, msg->posY, msg->posZ);
            transform->rotation = glm::vec3(msg->rotX, msg->rotY, msg->rotZ);
            transform->angleRotationRadians = 0;
            transform->scale = glm::vec3(1.f);

            // Asign visual meshes and materials
            if (!objItemListPtr->empty()) {
                auto* render = ecsPtr->AddComponent<MTRD::RenderComponent>(entity);
                render->meshes_ = &(*objItemListPtr)[netComp->meshId_].meshes;
                render->materials_ = &(*objItemListPtr)[netComp->meshId_].materials;
            }

            remoteEntities[msg->networkID] = entity;
            printf("Created remote entity for client %u\n", msg->networkID);
        } else {
            // Update existing remote player position and rotation
            size_t entity = it->second;
            auto* transform = ecsPtr->GetComponent<MTRD::TransformComponent>(entity);
            if (transform) {
                transform->position = glm::vec3(msg->posX, msg->posY, msg->posZ);
                transform->rotation = glm::vec3(msg->rotX, msg->rotY, msg->rotZ);
            }
        }
    }

}