#include <MotArda/Systems/RenderPbrSystem.hpp>
#include <MotArda/Debug.hpp>
#include <cstdio>

namespace MTRD {
    RenderPbrSystem::RenderPbrSystem(
        glm::mat4x4& vp,
        glm::mat4x4& model,
        glm::vec3& viewPos,
        bool& debug,
        int windowWidth,
        int windowHeight)
        : debug_(debug),
        viewPos_(viewPos),
        windowWidth_(windowWidth),
        windowHeight_(windowHeight),
        gBufferProgram{
            Shader::VertexFromFile("../assets/shaders/pbr_gbuffer_vertex.txt", debug),
            Shader::FragmentFromFile("../assets/shaders/pbr_gbuffer_fragment.txt", debug),
            debug },
        lightingProgram{
            Shader::VertexFromFile("../assets/shaders/pbr_lighting_vertex.txt", debug),
            Shader::FragmentFromFile("../assets/shaders/pbr_lighting_fragment.txt", debug),
            debug } {
        attributes = {
            { "position", 3, offsetof(Vertex, position), -1},
            { "uv", 2, offsetof(Vertex, uv), -1},
            { "normal", 3, offsetof(Vertex, normal), -1}
        };

        uniforms = {
            {"VP", -1, Window::UniformTypes::Mat4, glm::value_ptr(vp)},
            {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
        };

        InitGBuffer();
    }

    RenderPbrSystem::~RenderPbrSystem() {
        if (gBufferInitialized) {
            glDeleteFramebuffers(1, &gBufferFBO);
            glDeleteTextures(1, &gPosition);
            glDeleteTextures(1, &gNormal);
            glDeleteTextures(1, &gAlbedoMetallic);
            glDeleteTextures(1, &gRoughness);
            glDeleteRenderbuffers(1, &rboDepth);
        }
    }

    void RenderPbrSystem::SetShadowMaps(const std::vector<GLuint>& depthMaps) {
        depthMaps_ = depthMaps;
    }

    void RenderPbrSystem::SetShadowCubemaps(const std::vector<GLuint>& depthCubemaps) {
        depthCubemaps_ = depthCubemaps;
    }

    void RenderPbrSystem::InitGBuffer() {
        glCreateFramebuffers(1, &gBufferFBO);

        glCreateTextures(GL_TEXTURE_2D, 1, &gPosition);
        glTextureStorage2D(gPosition, 1, GL_RGBA16F, windowWidth_, windowHeight_);
        glTextureParameteri(gPosition, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(gPosition, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(gPosition, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(gPosition, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glNamedFramebufferTexture(gBufferFBO, GL_COLOR_ATTACHMENT0, gPosition, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &gNormal);
        glTextureStorage2D(gNormal, 1, GL_RGBA16F, windowWidth_, windowHeight_);
        glTextureParameteri(gNormal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(gNormal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(gBufferFBO, GL_COLOR_ATTACHMENT1, gNormal, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &gAlbedoMetallic);
        glTextureStorage2D(gAlbedoMetallic, 1, GL_RGBA8, windowWidth_, windowHeight_);
        glTextureParameteri(gAlbedoMetallic, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(gAlbedoMetallic, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(gBufferFBO, GL_COLOR_ATTACHMENT2, gAlbedoMetallic, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &gRoughness);
        glTextureStorage2D(gRoughness, 1, GL_R8, windowWidth_, windowHeight_);
        glTextureParameteri(gRoughness, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(gRoughness, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(gBufferFBO, GL_COLOR_ATTACHMENT3, gRoughness, 0);

        unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glNamedFramebufferDrawBuffers(gBufferFBO, 4, attachments);

        glCreateRenderbuffers(1, &rboDepth);
        glNamedRenderbufferStorage(rboDepth, GL_DEPTH_COMPONENT24, windowWidth_, windowHeight_);
        glNamedFramebufferRenderbuffer(gBufferFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckNamedFramebufferStatus(gBufferFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("PBR Framebuffer not complete!\n");
            std::abort();
        }

        gBufferInitialized = true;
    }

    void RenderPbrSystem::Resize(int width, int height) {
        windowWidth_ = width;
        windowHeight_ = height;

        if (gBufferInitialized) {
            glDeleteFramebuffers(1, &gBufferFBO);
            glDeleteTextures(1, &gPosition);
            glDeleteTextures(1, &gNormal);
            glDeleteTextures(1, &gAlbedoMetallic);
            glDeleteTextures(1, &gRoughness);
            glDeleteRenderbuffers(1, &rboDepth);
            gBufferInitialized = false;
        }

        InitGBuffer();
    }

    void RenderPbrSystem::GeometryPass(ECSManager& ecs, glm::mat4x4& model) {
        (void)ecs;
        (void)model;
    }

    void RenderPbrSystem::LightingPass(ECSManager& ecs, bool hasShadows) {
        (void)ecs;
        (void)hasShadows;
    }

    void RenderPbrSystem::RenderQuad() {
    }

    void RenderPbrSystem::Render(
        ECSManager& ecs,
        glm::mat4x4& model,
        bool hasShadows) {
        (void)ecs;
        (void)model;
        (void)hasShadows;
    }
}
