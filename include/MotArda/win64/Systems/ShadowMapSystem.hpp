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
        ShadowMapSystem(glm::mat4& model);

        void RenderShadowMap(
            ECSManager& ecs,
            glm::mat4& model
        );

        GLuint getDepthMap() const {
            return depthMaps_.empty() ? 0 : depthMaps_[0];
        }

        GLuint getDepthMap(size_t index) const {
            return (index < depthMaps_.size()) ? depthMaps_[index] : 0;
        }

        size_t getShadowMapCount() const {
            return depthMaps_.size();
        }

        std::vector<GLuint> getAllDepthMaps() const {
            return depthMaps_;
        }

        std::vector<VertexAttribute> attributes;
        std::vector<Window::UniformAttrib> uniforms;

    private:
        bool debug_;

        Program shadowProgram;
        std::vector<GLuint> depthMapFBOs_;
        std::vector<GLuint> depthMaps_;

        glm::mat4 lightSpaceMatrix_;

        const unsigned int SHADOW_WIDTH = 1024;
        const unsigned int SHADOW_HEIGHT = 1024;

        void CreateShadowMapResource(GLuint& fbo, GLuint& depthMap);
        void DrawCall(ECSManager& ecs, glm::mat4& model);
    };
}
