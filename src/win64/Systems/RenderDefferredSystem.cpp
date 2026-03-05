#include <MotArda/win64/Systems/RenderDefferredSystem.hpp>
#include <MotArda/win64/Debug.hpp>
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>
#include <string>
#include <vector>

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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth_, windowHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth_, windowHeight_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Framebuffer not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, mat.diffuseTexID);
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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "gPosition"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glUniform1i(glGetUniformLocation(lightingProgram.programId_, "gNormal"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
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

                glActiveTexture(GL_TEXTURE3);
                GLuint shadowTex = (current2DShadowIndex < depthMaps_.size()) ? depthMaps_[current2DShadowIndex] : 0;
                glBindTexture(GL_TEXTURE_2D, shadowTex);
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

                glActiveTexture(GL_TEXTURE3);
                GLuint shadowTex = (current2DShadowIndex < depthMaps_.size()) ? depthMaps_[current2DShadowIndex] : 0;
                glBindTexture(GL_TEXTURE_2D, shadowTex);
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

                glActiveTexture(GL_TEXTURE4);
                GLuint shadowCube = (currentCubeShadowIndex < depthCubemaps_.size()) ? depthCubemaps_[currentCubeShadowIndex] : 0;
                glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCube);
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

            glGenVertexArrays(1, &quadVAO);
            glBindVertexArray(quadVAO);

            glGenBuffers(1, &quadVBO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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