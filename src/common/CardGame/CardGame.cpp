#include <MotArda/CardGame/CardGame.hpp>
#include <MotArda/ObjItem.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Geometries.hpp>

namespace MTRD {
    Card::Card(std::shared_ptr<ObjItem> mesh, size_t entity, int suit, int number)
        : mesh(mesh), entity(entity), suit(suit), number(number) {
    }
    void CardGame::createCards(ECSManager& ecs, std::shared_ptr<ObjItem> cardMesh, MTRD::MotardaEng& eng) {
        for (int suit = 0; suit < 4; ++suit) {
            for (int num = 1; num <= 12; ++num) {
                size_t entity = ecs.AddEntity();

                auto* t = ecs.AddComponent<TransformComponent>(entity);
                t->position = glm::vec3(9999.0f, 9999.0f, 9999.0f);
                t->scale = glm::vec3(0.3f);
                t->rotation = glm::vec3(0.0f);
                t->angleRotationRadians = 0.0f;

                auto* r = ecs.AddComponent<RenderComponent>(entity);
                r->objitem_ = cardMesh;

                auto* physx = ecs.AddComponent<PhysxComponent>(entity);
                physx->shapeType = PhysxShapeType::Box;
                physx->halfExtents = glm::vec3(0.3f, 0.05f, 0.3f);
                physx->mass = 0.0f;
                physx->isDynamic = false;
                eng.createPhysxActor(*physx, *t);

                cards.emplace_back(cardMesh, entity, suit, num);
            }
        }
    }

    void CardGame::shuffleDeck()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
        usedCards.assign(cards.size(), false);
    }

    DealCardsPayload CardGame::dealThreeCards() {
        DealCardsPayload payload;
        int count = 0;
        for (int i = 0; i < cards.size() && count < 3; ++i) {
            if (!usedCards[i]) {
                payload.suit[count] = cards[i].suit;
                payload.value[count] = cards[i].number;
                usedCards[i] = true;
                count++;
            }
        }
        return payload;
    }

    void CardGame::drawSpecificCards(ECSManager& ecs, const DealCardsPayload& payload) {
        glm::vec3 handPositions[3] = {
            glm::vec3(-1.5f, 2.0f, 1.0f),
            glm::vec3(0.0f, 2.0f, 0.0f),
            glm::vec3(1.5f, 2.0f, 0.0f),
        };

        for (int i = 0; i < 3; ++i) {
            for (auto& card : cards) {
                if (card.suit == (int)payload.suit[i] && card.number == (int)payload.value[i]) {
                    auto* t = ecs.GetComponent<TransformComponent>(card.entity);
                    auto* physx = ecs.GetComponent<PhysxComponent>(card.entity);

                    if (t) t->position = handPositions[i];
                    if (physx && physx->actor) {
                        physx::PxTransform pxT(physx::PxVec3(
                            handPositions[i].x,
                            handPositions[i].y,
                            handPositions[i].z
                        ));
                        physx->actor->setGlobalPose(pxT);
                    }

                    playerHand.push_back(&card - &cards[0]);
                    printf("Carta recibida: %d de %d\n", card.number, card.suit);
                    break;
                }
            }
        }
    }
}
