#pragma once
#include <MotArda/Shader.hpp>
#include <MotArda/Program.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>
#include <MotArda/Components/LightComponent.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>

#include "../../deps/glm-master/glm/glm.hpp"
#include "../../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../../deps/glm-master/glm/gtc/type_ptr.hpp"

#include <vector>

namespace MTRD {
    class RenderLightsSystem {
    public:
        RenderLightsSystem(
            glm::mat4x4& vp,
            glm::mat4x4& model,
            glm::vec3& viewPos,
            bool& debug,
            int windowWidth = 800,
            int windowHeight = 600);

        void Render(
            ECSManager& ecs,
            glm::mat4x4& model,
            bool hasShadows = false
        );

        void SetShadowMap(GLuint depthMap);
        void SetShadowMaps(const std::vector<GLuint>& depthMaps);
        void SetShadowCubemaps(const std::vector<GLuint>& depthCubemaps);

        std::vector<VertexAttribute> attributes;
        std::vector<Window::UniformAttrib> uniforms;
        glm::mat4 lightSpaceMatrix_;

    private:
        bool debug_ = true;
        Program program;
        glm::vec3& viewPos_;
        glm::mat4x4& vp_;
        float shininess = 32.0f;
        float far_plane = 25.0f;
        std::vector<GLuint> depthMaps_;
        std::vector<GLuint> depthCubemaps_;
        int windowWidth_ = 800;
        int windowHeight_ = 600;

        void DrawCall(ECSManager& ecs, glm::mat4x4& model, size_t loc, const std::vector<size_t>& renderables, size_t shadowMapIndex = 0, bool isOmni = false);
    };
}