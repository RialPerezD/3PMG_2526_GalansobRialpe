#pragma once
#include <MotArda/win64/Shader.hpp>
#include <MotArda/win64/Program.hpp>
#include <MotArda/common/Components/TransformComponent.hpp>
#include <MotArda/common/Components/RenderComponent.hpp>
#include <MotArda/common/Components/LightComponent.hpp>
#include <MotArda/common/Ecs.hpp>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"


namespace MTRD {
    class RenderLightsSystem {
    public:
        RenderLightsSystem(glm::mat4x4& vp, glm::mat4x4& model, glm::vec3& viewPos, glm::mat4& lightSpaceMatrix);

        void Render(
            ECSManager& ecs,
            glm::mat4x4& model,
            bool debug
        );

        std::vector<VertexAttribute> attributes;
        std::vector<Window::UniformAttrib> uniforms;
    private:
        Program program;
        glm::vec3& viewPos_;
        float shininess = 32.0f;
    };
}
