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
    class ShadowMapSystem {
    public:
        ShadowMapSystem();
        ~ShadowMapSystem();

        void RenderShadowMap(
            ECSManager& ecs,
            std::vector<size_t> renderables,
            const glm::mat4& lightSpaceMatrix
        );

        std::vector<VertexAttribute> attributes;

        GLuint getShadowMap() const { return shadowMap_; }
        GLuint getShadowFBO() const { return shadowFBO_; }

    private:
        Program shadowProgram;
        GLuint shadowMap_;
        GLuint shadowFBO_;
        const unsigned int SHADOW_WIDTH = 2048;
        const unsigned int SHADOW_HEIGHT = 2048;
    };
}
