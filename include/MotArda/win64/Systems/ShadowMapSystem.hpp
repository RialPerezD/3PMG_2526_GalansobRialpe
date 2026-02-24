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

        void RenderShadowMap(
            ECSManager& ecs,
            const glm::mat4& lightSpaceMatrix
        );

        GLuint getDepthMap() const {
            return depthMap_;
		}

        std::vector<VertexAttribute> attributes;

    private:
        bool debug_;

        Program shadowProgram;
        GLuint depthMapFBO_;
        GLuint depthMap_;

        const unsigned int SHADOW_WIDTH = 1024;
        const unsigned int SHADOW_HEIGHT = 1024;
    };
}
