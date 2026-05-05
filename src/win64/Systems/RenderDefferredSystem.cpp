#include <MotArda/win64/Systems/RenderDefferredSystem.hpp>
#include <MotArda/win64/Debug.hpp>
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>
#include <string>
#include <vector>
#include <MotArda/common/Logger.hpp>

namespace MTRD {
    RenderDefferredSystem::RenderDefferredSystem(
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
            Shader::VertexFromFile("../assets/shaders/deferred_gbuffer_vertex.txt", debug),
            Shader::FragmentFromFile("../assets/shaders/deferred_gbuffer_fragment.txt", debug),
            debug },
            lightingProgram{
                Shader::VertexFromFile("../assets/shaders/deferred_lighting_vertex.txt", debug),
                Shader::FragmentFromFile("../assets/shaders/deferred_lighting_fragment.txt", debug),
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

    RenderDefferredSystem::~RenderDefferredSystem() {
        if (gBufferInitialized) {
            glDeleteFramebuffers(1, &gBufferFBO);
            glDeleteTextures(1, &gPosition);
            glDeleteTextures(1, &gNormal);
            glDeleteTextures(1, &gAlbedoSpec);
            glDeleteRenderbuffers(1, &rboDepth);
        }
    }

    void RenderDefferredSystem::SetShadowMaps(const std::vector<GLuint>& depthMaps) {
        depthMaps_ = depthMaps;
    }

    void RenderDefferredSystem::SetShadowCubemaps(const std::vector<GLuint>& depthCubemaps) {
        depthCubemaps_ = depthCubemaps;
    }

    void RenderDefferredSystem::InitGBuffer() {
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

        glCreateTextures(GL_TEXTURE_2D, 1, &gAlbedoSpec);
        glTextureStorage2D(gAlbedoSpec, 1, GL_RGBA8, windowWidth_, windowHeight_);
        glTextureParameteri(gAlbedoSpec, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(gAlbedoSpec, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(gBufferFBO, GL_COLOR_ATTACHMENT2, gAlbedoSpec, 0);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glNamedFramebufferDrawBuffers(gBufferFBO, 3, attachments);

        glCreateRenderbuffers(1, &rboDepth);
        glNamedRenderbufferStorage(rboDepth, GL_DEPTH_COMPONENT24, windowWidth_, windowHeight_);
        glNamedFramebufferRenderbuffer(gBufferFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckNamedFramebufferStatus(gBufferFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            MTRD::Logger::error("Framebuffer not complete!");
            std::abort();
        }

        gBufferInitialized = true;
    }

    void RenderDefferredSystem::Resize(int width, int height) {
        windowWidth_ = width;
        windowHeight_ = height;

        if (gBufferInitialized) {
            glDeleteFramebuffers(1, &gBufferFBO);
            glDeleteTextures(1, &gPosition);
            glDeleteTextures(1, &gNormal);
            glDeleteTextures(1, &gAlbedoSpec);
            glDeleteRenderbuffers(1, &rboDepth);
            gBufferInitialized = false;
        }

        InitGBuffer();
    }

    void RenderDefferredSystem::GeometryPass(ECSManager& ecs, glm::mat4x4& model) {
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(gBufferProgram.programId_);
        gBufferProgram.SetupAtributeLocations(attributes);

        for (size_t id : ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>()) {
            RenderComponent* render = ecs.GetComponent<RenderComponent>(id);
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(id);

            model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate(model, transform->angleRotationRadians, transform->rotation);
            }

            uniforms[1].values = glm::value_ptr(model);
            gBufferProgram.SetupUniforms(uniforms);

            for (size_t i = 0; i < render->meshes_->size(); i++) {
                Mesh* mesh = render->meshes_->at(i).get();

                if (mesh->materialId_ != -1) {
                    Material mat = render->materials_->at(mesh->materialId_);
                    if (!mat.loadeable) continue;

                    glBindTextureUnit(0, mat.diffuseTexID);
                    glUniform1i(glGetUniformLocation(gBufferProgram.programId_, "diffuseTexture"), 0);

                    glUniform3f(glGetUniformLocation(gBufferProgram.programId_, "DIFFUSE"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
                    glUniform3f(glGetUniformLocation(gBufferProgram.programId_, "SPECULAR"), mat.specular.x, mat.specular.y, mat.specular.z);
                    glUniform1f(glGetUniformLocation(gBufferProgram.programId_, "shininess"), shininess);
                }

                if (mesh->vao == GL_INVALID_INDEX || mesh->vao == 0) {
                    mesh->GenerateVao();
                    mesh->SetVertexAtribs(attributes);
                }
                glBindVertexArray(mesh->vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderDefferredSystem::LightingPass(ECSManager& ecs, bool hasShadows) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(lightingProgram.programId_);

        // Bind G-Buffer Textures
        glBindTextureUnit(0, gPosition);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "gPosition"), 0);

        glBindTextureUnit(1, gNormal);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "gNormal"), 1);

        glBindTextureUnit(2, gAlbedoSpec);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "gAlbedoSpec"), 2);

        // Common Uniforms
        glUniform3f(glGetUniformLocation(lightingProgram.programId_, "viewPos"), viewPos_.x, viewPos_.y, viewPos_.z);
        glUniform1f(glGetUniformLocation(lightingProgram.programId_, "shininess"), shininess);
        glUniform1f(glGetUniformLocation(lightingProgram.programId_, "far_plane"), far_plane);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "hasShadows"), hasShadows);

        auto lightEntities = ecs.GetEntitiesWithComponents<LightComponent>();
        LightComponent* mainLight = !lightEntities.empty() ? ecs.GetComponent<LightComponent>(lightEntities[0]) : nullptr;

        // --- 1. AMBIENT PASS ---
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "useAmbient"), 1);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "lightType"), 0);
        if (mainLight && mainLight->hasAmbient_) {
            glUniform3f(glGetUniformLocation(lightingProgram.programId_, "ambientColor"), mainLight->ambient_.color_.x, mainLight->ambient_.color_.y, mainLight->ambient_.color_.z);
            glUniform1f(glGetUniformLocation(lightingProgram.programId_, "ambientIntensity"), mainLight->ambient_.intensity_);
        }
        RenderQuad();

        // --- 2. LIGHTING PASSES (Additive) ---
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "useAmbient"), 0);

        size_t current2DShadowIndex = 0;
        size_t currentCubeShadowIndex = 0;

        for (size_t light_id : lightEntities) {
            LightComponent* lightComp = ecs.GetComponent<LightComponent>(light_id);

            // Directional Lights
            for (auto& dirLight : lightComp->directionalLights) {
                glUniform1i(glGetUniformLocation(lightingProgram.programId_, "lightType"), 1);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "lightDirOrPos"), dirLight.direction_.x, dirLight.direction_.y, dirLight.direction_.z);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "lightColor"), dirLight.color_.x, dirLight.color_.y, dirLight.color_.z);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "lightIntensity"), dirLight.intensity_);

                glm::mat4 lightSpace = dirLight.getLightSpaceMatrix();
                glUniformMatrix4fv(glGetUniformLocation(lightingProgram.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpace));

                GLuint shadowTex = (current2DShadowIndex < depthMaps_.size()) ? depthMaps_[current2DShadowIndex] : 0;
                glBindTextureUnit(3, shadowTex);
                glUniform1i(glGetUniformLocation(lightingProgram.programId_, "shadowTexture"), 3);

                RenderQuad();
                current2DShadowIndex++;
            }

            // Spot Lights
            for (auto& spot : lightComp->spotLights) {
                glUniform1i(glGetUniformLocation(lightingProgram.programId_, "lightType"), 2);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "lightDirOrPos"), spot.position_.x, spot.position_.y, spot.position_.z);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "spotLightDir"), spot.direction_.x, spot.direction_.y, spot.direction_.z);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "lightColor"), spot.color_.x, spot.color_.y, spot.color_.z);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "lightIntensity"), spot.intensity_);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "spotCutOff"), spot.cutOff_);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "spotOuterCutOff"), spot.outerCutOff_);

                glm::mat4 lightSpace = spot.getLightSpaceMatrix();
                glUniformMatrix4fv(glGetUniformLocation(lightingProgram.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpace));

                GLuint shadowTex = (current2DShadowIndex < depthMaps_.size()) ? depthMaps_[current2DShadowIndex] : 0;
                glBindTextureUnit(3, shadowTex);
                glUniform1i(glGetUniformLocation(lightingProgram.programId_, "shadowTexture"), 3);

                RenderQuad();
                current2DShadowIndex++;
            }

            // Point Lights
            for (auto& point : lightComp->pointLights) {
                glUniform1i(glGetUniformLocation(lightingProgram.programId_, "lightType"), 3);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "lightDirOrPos"), point.position_.x, point.position_.y, point.position_.z);
                glUniform3f(glGetUniformLocation(lightingProgram.programId_, "lightColor"), point.color_.x, point.color_.y, point.color_.z);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "lightIntensity"), point.intensity_);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "spotConstant"), point.constant_);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "spotLinear"), point.linear_);
                glUniform1f(glGetUniformLocation(lightingProgram.programId_, "spotQuadratic"), point.quadratic_);

                GLuint shadowCube = (currentCubeShadowIndex < depthCubemaps_.size()) ? depthCubemaps_[currentCubeShadowIndex] : 0;
                glBindTextureUnit(4, shadowCube);
                glUniform1i(glGetUniformLocation(lightingProgram.programId_, "shadowCubeMap"), 4);

                RenderQuad();
                currentCubeShadowIndex++;
            }
        }

        glDisable(GL_BLEND);
    }

    void RenderDefferredSystem::RenderQuad() {
        static unsigned int quadVAO = 0;
        static unsigned int quadVBO = 0;

        if (quadVAO == 0) {
            float quadVertices[] = {
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f
            };

            glCreateVertexArrays(1, &quadVAO);
            glCreateBuffers(1, &quadVBO);
            glNamedBufferData(quadVBO, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glVertexArrayVertexBuffer(quadVAO, 0, quadVBO, 0, 5 * sizeof(float));

            glEnableVertexArrayAttrib(quadVAO, 0);
            glVertexArrayAttribFormat(quadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(quadVAO, 0, 0);

            glEnableVertexArrayAttrib(quadVAO, 1);
            glVertexArrayAttribFormat(quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribBinding(quadVAO, 1, 0);
        }

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderDefferredSystem::Render(ECSManager& ecs, glm::mat4x4& model, bool hasShadows) {
        glViewport(0, 0, windowWidth_, windowHeight_);
        GeometryPass(ecs, model);
        LightingPass(ecs, hasShadows);
    }
}