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

    struct ChatMessage {
        uint32_t senderNetworkID;
        char text[256];
    };

    class NetworkSystem {
    public:
        using MessageCallback = std::function<void(uint32_t, const void*, size_t)>;
        using ChatCallback = std::function<void(const ChatMessage&)>;

        NetworkSystem(ECSManager& ecs,
            NetworkManager& netMgr,
            MessageCallback receiveCallback = nullptr);

        void Process();
        void SetChatCallback(ChatCallback callback) { chatCallback_ = callback; }

    private:
        void HandleReceive(uint32_t senderID, const void* data, size_t size);

        ECSManager& ecs_;
        NetworkManager& netMgr_;
        MessageCallback receiveCallback_;
        ChatCallback chatCallback_;
    };
}