#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <MotArda/Systems/NetworkSystem.hpp>

namespace MTRD {

    struct Card {
        int suit;   // 0 Oros, 1 Copas, 2 Espadas, 3 Bastos
        int number; // 1 al 12

        Card(int s, int n) : suit(s), number(n) {}
    };

    class CardGame {
    public:
        CardGame() = default;

        void initDeck();
        void shuffleDeck();

        // Métodos del Servidor para repartir
        DealCardsPayload dealThreeCards();
        DealCardsPayload dealOneCard();

        // Método del Cliente para recibir
        void receiveSpecificCards(const DealCardsPayload& payload);

        std::vector<Card> cards;
        std::vector<Card> playerHand;

    private:
        std::vector<bool> usedCards;
    };

    inline std::string GetSuitName(int suit) {
        switch (suit) {
        case 0: return "Oros";
        case 1: return "Copas";
        case 2: return "Espadas";
        case 3: return "Bastos";
        default: return "Desconocido";
        }
    }
}