#include "MotArda/Systems.hpp"

namespace MTRD {

    void Systems::RunRenderSystem(
        ECSManager& ecs,
        MotardaEng& eng,
        std::vector<Window::UniformAttrib>& uniforms,
        glm::mat4& model)
    {
        for (auto& rc : ecs.GetEntitiesWithComponents<Transform, Render>())
        {
            Transform* transform = ecs.GetComponent<Transform>(rc);
            Render* render = ecs.GetComponent<Render>(rc);

            model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            model = glm::rotate(model, transform->angleRotationRadians, transform->rotation
            );

            eng.windowOpenglSetUniformsValues(uniforms);
            eng.windowOpenglProgramUniformDrawRender(*render);
        }
    }

    void Systems::RunRenderSystemWithTraslations(
        ECSManager& ecs,
        MotardaEng& eng,
        std::vector<Window::UniformAttrib>& uniforms,
        glm::mat4& model)
    {
        static float rotateCounter = 0.f;
        for (auto& rc : ecs.GetEntitiesWithComponents<Transform, Render, Movement>())
        {
            // Get components
            Transform* transform = ecs.GetComponent<Transform>(rc);
            Render* render = ecs.GetComponent<Render>(rc);
            Movement* movement = ecs.GetComponent<Movement>(rc);

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

            // Send values
            eng.windowOpenglSetUniformsValues(uniforms);
            eng.windowOpenglProgramUniformDrawRender(*render);
        }
        rotateCounter += movSpeed;
        
        if (rotateCounter >= 2 || rotateCounter <= 0) {
            movSpeed *= -1;
        }
    }

}
