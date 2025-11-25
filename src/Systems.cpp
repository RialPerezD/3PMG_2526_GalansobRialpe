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

            eng.windowOpenglSetUniformsValues(uniforms);
            eng.windowOpenglProgramUniformDrawRender(*render);
        }
    }

    void Systems::RunRenderSystemWithTraslations(
        ECSManager& ecs,
        MotardaEng& eng,
        std::vector<Window::UniformAttrib>& uniforms,
        glm::mat4& model,
        std::vector<MTRD::Transform>& randomStats)
    {
        int i = 0;
        static float rotateCounter = 0.f;
        for (auto& rc : ecs.GetEntitiesWithComponents<Transform, Render>())
        {
            Transform* transform = ecs.GetComponent<Transform>(rc);
            Render* render = ecs.GetComponent<Render>(rc);

            glm::vec3 pos = transform->position + (randomStats[i].position * rotateCounter);
            model = glm::mat4(1.f);
            model = glm::translate(model, pos);
            model = glm::scale(model, transform->scale);
            glm::vec3 rot = transform->rotation += (randomStats[i].rotation * 0.01f);
            model = glm::rotate(model, rotateCounter, rot);

            eng.windowOpenglSetUniformsValues(uniforms);
            eng.windowOpenglProgramUniformDrawRender(*render);

            i++;
        }
        rotateCounter += movSpeed;
        
        if (rotateCounter >= 2 || rotateCounter <= 0) {
            movSpeed *= -1;
        }
    }

}
