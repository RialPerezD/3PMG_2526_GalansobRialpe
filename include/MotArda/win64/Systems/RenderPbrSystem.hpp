#pragma once
#include <MotArda/win64/Shader.hpp>
#include <MotArda/win64/Program.hpp>
#include <MotArda/common/Components/TransformComponent.hpp>
#include <MotArda/common/Components/RenderComponent.hpp>
#include <MotArda/common/Components/LightComponent.hpp>
#include <MotArda/common/Ecs.hpp>
#include "../../deps/glm-master/glm/glm.hpp"
#include "../../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../../deps/glm-master/glm/gtc/type_ptr.hpp"
#include <vector>
#include <glad/glad.h>

namespace MTRD {
    class RenderPbrSystem {
    public:
        RenderPbrSystem(
            glm::mat4x4& vp,
            glm::mat4x4& model,
            glm::vec3& viewPos,
            bool& debug,
            int windowWidth = 800,
            int windowHeight = 600);

        ~RenderPbrSystem();

        void Render(
            ECSManager& ecs,
            glm::mat4x4& model,
            bool hasShadows = false
        );

        void Resize(int width, int height);

        void SetShadowMaps(const std::vector<GLuint>& depthMaps);
        void SetShadowCubemaps(const std::vector<GLuint>& depthCubemaps);

        std::vector<VertexAttribute> attributes;
        std::vector<Window::UniformAttrib> uniforms;

    private:
        bool debug_ = true;
        Program gBufferProgram;
        Program lightingProgram;

        glm::vec3& viewPos_;
        float far_plane = 25.0f;
        int windowWidth_ = 800;
        int windowHeight_ = 600;

        GLuint gBufferFBO;
        GLuint gPosition;
        GLuint gNormal;
        GLuint gAlbedoMetallic;
        GLuint gRoughness;
        GLuint rboDepth;

        std::vector<GLuint> depthMaps_;
        std::vector<GLuint> depthCubemaps_;

        bool gBufferInitialized = false;

        void InitGBuffer();
        void GeometryPass(ECSManager& ecs, glm::mat4x4& model);
        void LightingPass(ECSManager& ecs, bool hasShadows);
        void RenderQuad();
    };
}
