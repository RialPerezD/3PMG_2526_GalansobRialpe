#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <glad/glad.h>
#include <MotArda/ObjItem.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Components/PhysxComponent.hpp>
#include <MotArda/Systems/NetworkSystem.hpp>
#include <MotArda/Engine.hpp>

namespace MTRD {

    class Card {
    public:
        Card() = default;
        Card(std::shared_ptr<ObjItem> mesh, size_t entity, int suit, int number);

        std::shared_ptr<ObjItem> mesh;
        size_t entity;
        int suit;            // 0 Oros, 1 Copas, 2 Espadas, 3 Bastos
        int number;
    };

    class CardGame {
    public:
        CardGame() = default;

        void createCards(ECSManager& ecs, std::shared_ptr<ObjItem> cardMesh, MTRD::MotardaEng& eng);
        void shuffleDeck();
        DealCardsPayload dealThreeCards();

        void drawSpecificCards(ECSManager& ecs, const DealCardsPayload& payload);

        std::vector<Card> cards;
        std::vector<int> playerHand;

    private:
        // Cards that have already been given
        std::vector<bool> usedCards;
    };

}