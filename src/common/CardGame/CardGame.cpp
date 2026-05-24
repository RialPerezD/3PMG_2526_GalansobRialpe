#include <MotArda/CardGame/CardGame.hpp>
#include <MotArda/Logger.hpp>
#include <iostream>

namespace MTRD {

    void CardGame::initDeck() {
        cards.clear();
        for (int suit = 0; suit < 4; ++suit) {
            for (int num = 1; num <= 12; ++num) {
                cards.emplace_back(suit, num);
            }
        }
        usedCards.assign(cards.size(), false);
    }

    void CardGame::shuffleDeck() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
        std::fill(usedCards.begin(), usedCards.end(), false);
    }

    DealCardsPayload CardGame::dealThreeCards() {
        DealCardsPayload payload;
        // Limpiamos la estructura asegurando ceros
        for (int i = 0; i < 3; ++i) { payload.suit[i] = 0; payload.value[i] = 0; }

        int count = 0;
        for (size_t i = 0; i < cards.size() && count < 3; ++i) {
            if (!usedCards[i]) {
                payload.suit[count] = cards[i].suit;
                payload.value[count] = cards[i].number;
                usedCards[i] = true;
                count++;
            }
        }
        return payload;
    }

    // --- NUEVA FUNCIÓN PARA SACAR SOLO 1 CARTA DEL MAZO ---
    DealCardsPayload CardGame::dealOneCard() {
        DealCardsPayload payload;
        // Ponemos todo a 0 (0 significa vacío en el protocolo que diseñaremos)
        for (int i = 0; i < 3; ++i) { payload.suit[i] = 0; payload.value[i] = 0; }

        for (size_t i = 0; i < cards.size(); ++i) {
            if (!usedCards[i]) {
                payload.suit[0] = cards[i].suit;
                payload.value[0] = cards[i].number; // Solo llenamos el primer slot
                usedCards[i] = true;
                break;
            }
        }
        return payload;
    }

    // --- RECEPTOR MEJORADO QUE ACUMULA EN LA MANO LOCAL ---
    void CardGame::receiveSpecificCards(const DealCardsPayload& payload) {
        MTRD::Logger::info("--- NUEVAS CARTAS LLEGANDO POR RED ---");

        for (int i = 0; i < 3; ++i) {
            int s = static_cast<int>(payload.suit[i]);
            int v = static_cast<int>(payload.value[i]);

            // Si el valor es 0, ignoramos este slot (está vacío o es basura de red)
            if (v <= 0 || v > 12) continue;

            // Añadimos de verdad la carta al vector dinámico 'playerHand'
            playerHand.emplace_back(s, v);
            MTRD::Logger::info("-> Añadida a tu mano local: {} de {}", v, GetSuitName(s));
        }

        // Imprimimos el estado real actual de tu mano por consola
        MTRD::Logger::info("--- ESTADO DE TU MANO ACTUAL (Total: {}) ---", playerHand.size());
        for (size_t i = 0; i < playerHand.size(); ++i) {
            MTRD::Logger::info("[{}] {} de {}", i + 1, playerHand[i].number, GetSuitName(playerHand[i].suit));
        }
        MTRD::Logger::info("--------------------------------------------");
    }
    uint32_t CardGame::resolveBaza(int triunfo)
    {
        if (tableCards.size() < 2) return 0;

        // El primer jugador marca el palo de salida
        int leadSuit = tableCards[0].card.suit;

        uint32_t winnerID = tableCards[0].playerID;
        const Card* winnerCard = &tableCards[0].card;
        int winnerStrength = GetBriscaStrength(winnerCard->number);
        bool winnerIsTriunfo = (winnerCard->suit == triunfo);

        for (size_t i = 1; i < tableCards.size(); ++i) {
            const Card& c = tableCards[i].card;
            bool isTriunfo = (c.suit == triunfo);
            int strength = GetBriscaStrength(c.number);

            bool beats = false;
            if (isTriunfo && !winnerIsTriunfo) {
                beats = true;  // triunfo gana a cualquier otro palo
            } else if (isTriunfo && winnerIsTriunfo) {
                beats = (strength > winnerStrength);  // ambos triunfo || mayor fuerza
            } else if (c.suit == leadSuit && !winnerIsTriunfo) {
                beats = (strength > winnerStrength);  // mismo palo salida, mayor fuerza
            }
            // otro palo sin ser triunfo || no puede ganar

            if (beats) {
                winnerID = tableCards[i].playerID;
                winnerCard = &tableCards[i].card;
                winnerStrength = strength;
                winnerIsTriunfo = isTriunfo;
            }
        }

        // Sumar puntos al ganador
        int totalPoints = 0;
        for (auto& pc : tableCards)
            totalPoints += GetBriscaPoints(pc.card.number);
        if (winnerID < 5)
        {
            scores[winnerID] += totalPoints;
        }

        tableCards.clear();
        return winnerID;
    }
    
}