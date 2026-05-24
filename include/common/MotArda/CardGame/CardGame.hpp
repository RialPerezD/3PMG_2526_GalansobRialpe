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

        // Played cards info - for server only
        struct PlayedCard {
            uint32_t playerID;
            Card card;
        };
        std::vector<PlayedCard> tableCards; // Currently played cards
        int scores[5] = { 0 }; // points per playerID (4 player max)

        uint32_t resolveBaza(int triunfo);

    private:
        std::vector<bool> usedCards;
    };

    inline int GetBriscaPoints(int number) {
        switch (number) {
        case 1: return 11;
        case 3: return 10;
        case 12: return 4;
        case 11: return 3;
        case 10: return 2;
        default: return 0;
        }
    }

    inline int GetBriscaStrength(int number) {
        switch (number) {
        case 1:  return 8;
        case 3:  return 7;
        case 12: return 6;
        case 11: return 5;
        case 10: return 4;
        default: return number - 3; // 4->1, 5->2, 6->3, 7->4
        }
    }

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