#pragma once
#include <MotArda/Shader.hpp>
#include <MotArda/Program.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>
#include <MotArda/Components/LightComponent.hpp>
#include <MotArda/Ecs.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace MTRD {
    class ShadowMapSystem {
    public:
        ShadowMapSystem(glm::mat4& model, bool& debug);

        void RenderShadowMap(ECSManager& ecs, glm::mat4& model);

        std::vector<GLuint> getAllDepthMaps() const { return depthMaps_; }
        std::vector<GLuint> getAllDepthCubemaps() const { return depthCubemaps_; }

        std::vector<VertexAttribute> attributes;
        std::vector<Window::UniformAttrib> uniforms;

    private:
        bool debug_ = true;
        Program shadowProgram;
        Program omniShadowProgram;

        std::vector<GLuint> depthMapFBOs_;
        std::vector<GLuint> depthMaps_;

        std::vector<GLuint> omniDepthFBOs_;
        std::vector<GLuint> depthCubemaps_;

        glm::mat4 lightSpaceMatrix_;
        const unsigned int SHADOW_WIDTH = 1024;
        const unsigned int SHADOW_HEIGHT = 1024;

        void CreateShadowMapResource(GLuint& fbo, GLuint& depthMap);
        void CreateOmniShadowResource(GLuint& fbo, GLuint& cubemap);
        void DrawCall(ECSManager& ecs, glm::mat4& model, Program& currentProg);
    };
}
