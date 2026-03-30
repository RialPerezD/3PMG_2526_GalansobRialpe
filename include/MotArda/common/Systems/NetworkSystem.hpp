#pragma once
#include "../Ecs.hpp"
#include "../Engine.hpp"
#include "../NetworkManager.hpp"
#include <functional>

namespace MTRD {

    struct NetworkMessage {
        uint32_t networkID;
        float posX, posY, posZ;
        float rotX, rotY, rotZ;
    };

    class NetworkSystem {
    public:
        static void Process(ECSManager& ecs, MotardaEng& eng, NetworkManager& netMgr,
            std::function<void(uint32_t, const void*, size_t)> receiveCallback = nullptr);

    private:
        static void HandleReceive(uint32_t senderID, const void* data, size_t size);
    };
}
