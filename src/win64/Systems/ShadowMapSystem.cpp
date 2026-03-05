#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Debug.hpp>

namespace MTRD {
    ShadowMapSystem::ShadowMapSystem(glm::mat4& model, bool& debug)
        : debug_(debug),
        shadowProgram{
            Shader::VertexFromFile("../assets/shaders/shadow_map_vertex.txt", debug),
            Shader::FragmentFromFile("../assets/shaders/shadow_map_fragment.txt", debug),
            debug
        },
        omniShadowProgram{
            Shader::VertexFromFile("../assets/shaders/omni_shadow_vertex.txt", debug),
            Shader::GeometryFromFile("../assets/shaders/omni_shadow_geom.txt", debug),
            Shader::FragmentFromFile("../assets/shaders/omni_shadow_frag.txt", debug),
            debug
        }
    {
        attributes = {
            { "position", 3, offsetof(Vertex, position), -1},
            { "uv", 2, offsetof(Vertex, uv), -1},
            { "normal", 3, offsetof(Vertex, normal), -1}
        };

        uniforms = {
            {"lightSpaceMatrix", -1, Window::UniformTypes::Mat4, glm::value_ptr(lightSpaceMatrix_)},
            {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
        };
    }

    void ShadowMapSystem::CreateShadowMapResource(GLuint& fbo, GLuint& depthMap) {
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMapSystem::CreateOmniShadowResource(GLuint& fbo, GLuint& cubemap) {
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMapSystem::RenderShadowMap(ECSManager& ecs, glm::mat4& model) {
        auto lightEntities = ecs.GetEntitiesWithComponents<LightComponent>();
        size_t dirSpotCount = 0;
        size_t pointCount = 0;

        for (size_t id : lightEntities) {
            auto* l = ecs.GetComponent<LightComponent>(id);
            dirSpotCount += l->directionalLights.size() + l->spotLights.size();
            pointCount += l->pointLights.size();
        }

        if (depthMaps_.size() != dirSpotCount) {
            for (auto fbo : depthMapFBOs_) glDeleteFramebuffers(1, &fbo);
            for (auto tex : depthMaps_) glDeleteTextures(1, &tex);
            depthMapFBOs_.clear();
            depthMaps_.clear();
            for (size_t i = 0; i < dirSpotCount; i++) {
                GLuint fbo, tex;
                CreateShadowMapResource(fbo, tex);
                depthMapFBOs_.push_back(fbo);
                depthMaps_.push_back(tex);
            }
        }

        if (depthCubemaps_.size() != pointCount) {
            for (auto fbo : omniDepthFBOs_) glDeleteFramebuffers(1, &fbo);
            for (auto cb : depthCubemaps_) glDeleteTextures(1, &cb);
            omniDepthFBOs_.clear();
            depthCubemaps_.clear();
            for (size_t i = 0; i < pointCount; i++) {
                GLuint fbo, cb;
                CreateOmniShadowResource(fbo, cb);
                omniDepthFBOs_.push_back(fbo);
                depthCubemaps_.push_back(cb);
            }
        }

        size_t current2D = 0;
        size_t currentCube = 0;
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glEnable(GL_DEPTH_TEST);

        for (size_t id : lightEntities) {
            auto* l = ecs.GetComponent<LightComponent>(id);

            glUseProgram(shadowProgram.programId_);
            for (auto& dl : l->directionalLights) {
                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs_[current2D]);
                glClear(GL_DEPTH_BUFFER_BIT);
                lightSpaceMatrix_ = dl.getLightSpaceMatrix();
                DrawCall(ecs, model, shadowProgram);
                current2D++;
            }
            for (auto& sl : l->spotLights) {
                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs_[current2D]);
                glClear(GL_DEPTH_BUFFER_BIT);
                lightSpaceMatrix_ = sl.getLightSpaceMatrix();
                DrawCall(ecs, model, shadowProgram);
                current2D++;
            }

            glUseProgram(omniShadowProgram.programId_);
            for (auto& pl : l->pointLights) {
                glBindFramebuffer(GL_FRAMEBUFFER, omniDepthFBOs_[currentCube]);
                glClear(GL_DEPTH_BUFFER_BIT);

                float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
                float nearPlane = 1.0f;
                float farPlane = 25.0f;
                glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
                glm::vec3 p = pl.position_;

                std::vector<glm::mat4> transforms;
                transforms.push_back(proj * glm::lookAt(p, p + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)));
                transforms.push_back(proj * glm::lookAt(p, p + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));
                transforms.push_back(proj * glm::lookAt(p, p + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
                transforms.push_back(proj * glm::lookAt(p, p + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));
                transforms.push_back(proj * glm::lookAt(p, p + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));
                transforms.push_back(proj * glm::lookAt(p, p + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));

                for (int i = 0; i < 6; ++i) {
                    glUniformMatrix4fv(glGetUniformLocation(omniShadowProgram.programId_, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(transforms[i]));
                }
                glUniform3f(glGetUniformLocation(omniShadowProgram.programId_, "lightPos"), p.x, p.y, p.z);
                glUniform1f(glGetUniformLocation(omniShadowProgram.programId_, "far_plane"), farPlane);

                DrawCall(ecs, model, omniShadowProgram);
                currentCube++;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMapSystem::DrawCall(ECSManager& ecs, glm::mat4& model, Program& currentProg) {
        auto renderables = ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>();
        for (size_t id : renderables) {
            RenderComponent* render = ecs.GetComponent<RenderComponent>(id);
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(id);

            model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate(model, transform->angleRotationRadians, transform->rotation);
            }

            currentProg.SetupUniforms(uniforms);
            currentProg.SetupAtributeLocations(attributes);

            for (size_t i = 0; i < render->meshes_->size(); i++) {
                Mesh* mesh = render->meshes_->at(i).get();
                if (mesh->vao == GL_INVALID_INDEX || mesh->vao == 0) {
                    mesh->GenerateVao();
                    mesh->SetVertexAtribs(attributes);
                }
                glBindVertexArray(mesh->vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));

                mesh->vao = GL_INVALID_INDEX;
            }
        }
    }
}