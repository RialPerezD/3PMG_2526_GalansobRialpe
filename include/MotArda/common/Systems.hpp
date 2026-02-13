#pragma once
#include "../deps/glm-master/glm/ext/matrix_transform.hpp"
#include "Ecs.hpp"
#include "Engine.hpp"

namespace MTRD {

    class Systems {
    public:
        /**
        * @brief RunRenderSystem
        * @param ECSManager& ecs Ecs that contains all the entities.
        * @param MotardaEng& eng Reference to our engine.
        * @param std::vector<Window::UniformAttrib>& uniforms Sends an 
        * uniform vector to the shader.
        * @param glm::mat4& model Mat4 type object.
        * @details First, it gets all the entities with the Transform
        * and Render component (thanks to the ECS), then it sets the 
        * position, scale and rotation of the model.
        */
        void TranslationSystem(
            ECSManager& ecs,
            MotardaEng& eng,
            std::vector<Window::UniformAttrib>& uniforms,
            glm::mat4& model
        );

        /**
        * @brief RunRenderSystemWithTraslations
        * @param ECSManager& ecs Ecs that contains all the entities.
        * @param MotardaEng& eng Reference to our engine.
        * @param std::vector<Window::UniformAttrib>& uniforms Sends an
        * uniform vector to the shader.
        * @param glm::mat4& model Mat4 type object.
        * @details Allows the object to move and rotate.
        */
        void TranslationSystem2(
            ECSManager& ecs,
            MotardaEng& eng,
            std::vector<Window::UniformAttrib>& uniforms,
            glm::mat4& model
        );

    private:
        //< Movement speed of the object
        float movSpeed = 0.005f;
    };

}
