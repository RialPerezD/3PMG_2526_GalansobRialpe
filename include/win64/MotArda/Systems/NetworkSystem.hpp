#pragma once

#include <MotArda/Ecs.hpp>
#include <MotArda/NetworkManager.hpp>

#include <functional>
#include <cstdint>

namespace MTRD {

    enum class MessageType : uint8_t {
        EntityUpdate,
        Chat,
        ConnectionStatus,
        Action,
        CardPlay,
        DealCards,
        CardResult
    };

#pragma pack(push, 1)
    struct NetMessage {
        MessageType type;
        uint32_t senderId;
    };

    struct EntityUpdatePayload {
        uint32_t networkID;
        float meshId_;
        float posX, posY, posZ;
        float rotX, rotY, rotZ;
    };

    struct CardPayload {
        uint32_t suit;
        uint32_t value;
    };
     
    // Payload with 3 cards (hand), suit and value
    struct DealCardsPayload {
        uint32_t suit[3];
        uint32_t value[3];
    };

    struct CardResultPayload {
        uint32_t winnerID;   
        uint32_t points;     
        uint32_t card1Suit;  uint32_t card1Value; 
        uint32_t card2Suit;  uint32_t card2Value;
        uint32_t card3Suit;  uint32_t card3Value;
        uint32_t card4Suit;  uint32_t card4Value;

    };

    struct CardResultPacket {
        NetMessage header;
        CardResultPayload payload;
    };

    struct DealCardsPacket {
        NetMessage header;
        DealCardsPayload payload;
    };

    struct CardPacket {
        NetMessage header;
        CardPayload payload;
    };

    struct ChatPayload {
        char text[256];
    };
#pragma pack(pop)

    class NetworkSystem {
    public:
        using MessageCallback = std::function<void(uint32_t, const void*, size_t)>;
        using ChatCallback = std::function<void(uint32_t, const ChatPayload&)>;

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