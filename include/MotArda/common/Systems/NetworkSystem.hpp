#pragma once
#include "../Ecs.hpp"
#include "../NetworkManager.hpp"
#include <functional>

namespace MTRD {

    struct NetworkMessage {
        uint32_t networkID;
        float meshId_;
        float posX, posY, posZ;
        float rotX, rotY, rotZ;
    };

    class NetworkSystem {
    public:
        using MessageCallback = std::function<void(uint32_t, const void*, size_t)>;

        NetworkSystem(ECSManager& ecs,
            NetworkManager& netMgr,
            MessageCallback receiveCallback = nullptr);

        void Process();

    private:
        void HandleReceive(uint32_t senderID, const void* data, size_t size);

        ECSManager& ecs_;
        NetworkManager& netMgr_;
        MessageCallback receiveCallback_;
    };
}