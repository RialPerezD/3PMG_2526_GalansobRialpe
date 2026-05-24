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
        glGenFramebuffers(1, &gBufferFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth_, windowHeight_, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth_, windowHeight_, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        glGenTextures(1, &gAlbedoMetallic);
        glBindTexture(GL_TEXTURE_2D, gAlbedoMetallic);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth_, windowHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoMetallic, 0);

        glGenTextures(1, &gRoughness);
        glBindTexture(GL_TEXTURE_2D, gRoughness);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, windowWidth_, windowHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gRoughness, 0);

        unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth_, windowHeight_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("PBR Framebuffer not complete!\n");
            std::abort();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
