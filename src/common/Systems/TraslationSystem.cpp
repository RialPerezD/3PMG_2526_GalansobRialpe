#include "MotArda/common/Systems/TraslationSystem.hpp"

namespace MTRD {

    void TranslationSystem::TranslationSystemPlane(
        ECSManager& ecs,
        MotardaEng& eng,
        glm::mat4& model)
    {
        for (auto& rc : ecs.GetEntitiesWithComponents<TransformComponent, RenderComponent>())
        {
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(rc);
            RenderComponent* render = ecs.GetComponent<RenderComponent>(rc);

            model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate( model, transform->angleRotationRadians, transform->rotation);
            }
        }
    }


    void TranslationSystem::TranslationSystemWithMovementComponent(
        ECSManager& ecs,
        MotardaEng& eng,
        glm::mat4& model)
    {
        static float rotateCounter = 0.f;
        for (auto& rc : ecs.GetEntitiesWithComponents<TransformComponent, RenderComponent, MovementComponent>())
        {
            // Get components
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(rc);
            RenderComponent* render = ecs.GetComponent<RenderComponent>(rc);
            MovementComponent* movement = ecs.GetComponent<MovementComponent>(rc);

            // Reset Model Matrix
            model = glm::mat4(1.f);

            // Move in space, auto movement if needed
            if (movement->shouldConstantMove) {
                model = glm::translate(
                    model,
                    transform->position + (movement->position * rotateCounter)
                );
            } else {
                model = glm::translate(
                    model,
                    transform->position + movement->position
                );
            }

            // Scale item
            if (glm::length(movement->scale) != 0) {
                model = glm::scale(
                    model,
                    transform->scale + movement->scale
                );
            } else {
                model = glm::scale(
                    model,
                    transform->scale
                );
            }

            // Rotate item, firs in local to apply transform rotation and then movmenet rotation
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate(
                    model,
                    transform->angleRotationRadians, transform->rotation
                );
            }

            if(glm::length(movement->rotation) != 0){
                if (movement->shouldConstantMove) {
                    model = glm::rotate(
                        model,
                        rotateCounter, movement->rotation
                    );
                } else {
                    model = glm::rotate(
                        model,
                        movement->angleRotationRadians, movement->rotation
                    );
                }
            }
        }
        rotateCounter += movSpeed;
        
        if (rotateCounter >= 2 || rotateCounter <= 0) {
            movSpeed *= -1;
        }
    }

}
