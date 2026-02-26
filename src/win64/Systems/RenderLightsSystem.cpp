#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Debug.hpp>
#include <string>
#include <vector>


namespace MTRD {
    RenderLightsSystem::RenderLightsSystem(glm::mat4x4& vp, glm::mat4x4& model, glm::vec3& viewPos)
        : program{
            Shader::VertexFromFile("../assets/shaders/textured_lights_obj_vertex.txt",true) ,
            Shader::FragmentFromFile("../assets/shaders/textured_lights_obj_fragment.txt", true),true }
        , viewPos_(viewPos)
    {
        attributes = {
            { "position", 3, offsetof(Vertex, position), -1},
            { "uv", 2, offsetof(Vertex, uv), -1},
            { "normal", 3, offsetof(Vertex, normal), -1}
        };

        uniforms = {
            {"VP", -1, Window::UniformTypes::Mat4, glm::value_ptr(vp)},
            {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
            {"lightSpaceMatrix", -1, Window::UniformTypes::Mat4, glm::value_ptr(lightSpaceMatrix_)},
        };
    }

    void RenderLightsSystem::SetShadowMap(GLuint depthMap) {
        depthMaps_.clear();
        if (depthMap != 0) {
            depthMaps_.push_back(depthMap);
        }
    }

    void RenderLightsSystem::SetShadowMaps(const std::vector<GLuint>& depthMaps) {
        depthMaps_ = depthMaps;
    }


    void RenderLightsSystem::DrawCall(ECSManager& ecs, glm::mat4x4& model, size_t loc, const std::vector<size_t>& renderables, size_t shadowMapIndex) {
        for (size_t id : renderables) {
            RenderComponent* render = ecs.GetComponent<RenderComponent>(id);
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(id);

            model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate(model, transform->angleRotationRadians, transform->rotation);
            }
            program.SetupUniforms(uniforms);

            glUniform1i(glGetUniformLocation(program.programId_, "diffuseTexture"), 0);
            glUniform1i(glGetUniformLocation(program.programId_, "shadowTexture"), 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            GLuint shadowTex = (shadowMapIndex < depthMaps_.size()) ? depthMaps_[shadowMapIndex] : 0;
            glBindTexture(GL_TEXTURE_2D, shadowTex);

            for (size_t i = 0; i < render->meshes_->size(); i++) {
                Mesh* mesh = render->meshes_->at(i).get();

                if (mesh->materialId_ != -1) {
                    Material mat = render->materials_->at(mesh->materialId_);

                    if (!mat.loadeable) continue;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, mat.diffuseTexID);
                    glUniform1i(loc, 0);

                    glUniform3f(glGetUniformLocation(program.programId_, "DIFFUSE"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "SPECULAR"), mat.specular.x, mat.specular.y, mat.specular.z);

                    glCheckError();
                }

                if (mesh->vao == GL_INVALID_INDEX || mesh->vao == 0) {
                    mesh->GenerateVao();
                    mesh->SetVertexAtribs(attributes);
                }
                glBindVertexArray(mesh->vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));

                glCheckError();
            }
        }
    }


    void RenderLightsSystem::Render(
        ECSManager& ecs,
        glm::mat4x4& model,
        bool hasShadows,
        bool debug
    ) {
        glUseProgram(program.programId_);

        glEnable(GL_DEPTH_TEST);

        size_t loc = glGetUniformLocation(program.programId_, "diffuseTexture");
        program.SetupAtributeLocations(attributes);

        auto lightEntities = ecs.GetEntitiesWithComponents<LightComponent>();
        LightComponent* light = nullptr;
        if (!lightEntities.empty()) {
            light = ecs.GetComponent<LightComponent>(lightEntities[0]);
        }

        glUniform3f(glGetUniformLocation(program.programId_, "viewPos"), viewPos_.x, viewPos_.y, viewPos_.z);
        glUniform1f(glGetUniformLocation(program.programId_, "shininess"), shininess);
        glUniform1i(glGetUniformLocation(program.programId_, "hasShadows"), hasShadows);

        if (light && light->hasAmbient_) {
            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 1);
            glUniform3f(glGetUniformLocation(program.programId_, "ambientColor"), light->ambient_.color_.x, light->ambient_.color_.y, light->ambient_.color_.z);
            glUniform1f(glGetUniformLocation(program.programId_, "ambientIntensity"), light->ambient_.intensity_);
        } else {
            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 0);
        }

        glUniform1i(glGetUniformLocation(program.programId_, "diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(program.programId_, "shadowTexture"), 1);

        auto renderables = ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>();

        if (!renderables.empty()) {
            glDepthMask(GL_TRUE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 0);
            glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 0);
            lightSpaceMatrix_ = glm::mat4(0);
            glUniformMatrix4fv(glGetUniformLocation(program.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix_));
            DrawCall(ecs, model, loc, renderables, 0);

            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            size_t currentShadowMapIndex = 0;

            for (size_t light_id : lightEntities) {
                LightComponent* lightComp = ecs.GetComponent<LightComponent>(light_id);

                for (auto& dirLight : lightComp->directionalLights) {
                    glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 0);
                    glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 1);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightDirOrPos"), dirLight.direction_.x, dirLight.direction_.y, dirLight.direction_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightColor"), dirLight.color_.x, dirLight.color_.y, dirLight.color_.z);
                    glUniform1f(glGetUniformLocation(program.programId_, "lightIntensity"), dirLight.intensity_);

                    lightSpaceMatrix_ = dirLight.getLightSpaceMatrix();
                    glUniformMatrix4fv(glGetUniformLocation(program.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix_));

                    DrawCall(ecs, model, loc, renderables, currentShadowMapIndex);
                    currentShadowMapIndex++;
                }

                for (auto& spot : lightComp->spotLights) {
                    glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 0);
                    glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 2);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightDirOrPos"), spot.position_.x, spot.position_.y, spot.position_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "spotLightDir"), spot.direction_.x, spot.direction_.y, spot.direction_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightColor"), spot.color_.x, spot.color_.y, spot.color_.z);
                    glUniform1f(glGetUniformLocation(program.programId_, "lightIntensity"), spot.intensity_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotCutOff"), spot.cutOff_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotOuterCutOff"), spot.outerCutOff_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotConstant"), spot.constant_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotLinear"), spot.linear_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotQuadratic"), spot.quadratic_);

                    lightSpaceMatrix_ = spot.getLightSpaceMatrix();
                    glUniformMatrix4fv(glGetUniformLocation(program.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix_));

                    DrawCall(ecs, model, loc, renderables, currentShadowMapIndex);
                    currentShadowMapIndex++;
                }
            }

            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }
    }
}
