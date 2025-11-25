#pragma once
#include "../deps/glm-master/glm/ext/matrix_transform.hpp"
#include "Ecs.hpp"
#include "MotArda/Engine.hpp"

namespace MTRD {

    class Systems {
    public:
        void RunRenderSystem(
            ECSManager& ecs,
            MotardaEng& eng,
            std::vector<Window::UniformAttrib>& uniforms,
            glm::mat4& model
        );

        void RunRenderSystemWithTraslations(
            ECSManager& ecs,
            MotardaEng& eng,
            std::vector<Window::UniformAttrib>& uniforms,
            glm::mat4& model,
            std::vector<MTRD::Transform>& randomStats
        );

    private:
        float movSpeed = 0.005f;
    };

}
