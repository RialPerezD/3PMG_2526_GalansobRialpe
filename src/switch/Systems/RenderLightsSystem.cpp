#include <MotArda/Systems/RenderLightsSystem.hpp>
#include <MotArda/Debug.hpp>
#include <string>
#include <vector>
#include <cstdio>
#include <cmath>

namespace MTRD {
    RenderLightsSystem::RenderLightsSystem(glm::mat4x4& vp,
        glm::mat4x4& model,
        glm::vec3& viewPos,
        bool& debug,
        int windowWidth,
        int windowHeight)
        : debug_(debug),
        program{
            Shader::VertexFromFile("romfs:/shadersSwitch/textured_lights_obj_vertex.txt", debug) ,
            Shader::FragmentFromFile("romfs:/shadersSwitch/textured_lights_obj_fragment.txt", debug),
            debug }
            , viewPos_(viewPos),
        vp_(vp),
        windowWidth_(windowWidth),
        windowHeight_(windowHeight) {
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

    void RenderLightsSystem::SetShadowCubemaps(const std::vector<GLuint>& depthCubemaps) {
        depthCubemaps_ = depthCubemaps;
    }

    void RenderLightsSystem::DrawCall(ECSManager& ecs, glm::mat4x4& model, size_t loc, const std::vector<size_t>& renderables, size_t shadowMapIndex, bool isOmni) {
        Frustum frustum;
        {
            const glm::mat4& vp = vp_;
            frustum.planes[0].normal = glm::vec3(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0]);
            frustum.planes[0].d = vp[3][3] + vp[3][0];
            frustum.planes[1].normal = glm::vec3(vp[0][3] - vp[0][0], vp[1][3] - vp[1][2], vp[2][3] - vp[2][0]);
            frustum.planes[1].d = vp[3][3] - vp[3][0];
            frustum.planes[2].normal = glm::vec3(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1]);
            frustum.planes[2].d = vp[3][3] + vp[3][1];
            frustum.planes[3].normal = glm::vec3(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1]);
            frustum.planes[3].d = vp[3][3] - vp[3][1];
            frustum.planes[4].normal = glm::vec3(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2]);
            frustum.planes[4].d = vp[3][3] + vp[3][2];
            frustum.planes[5].normal = glm::vec3(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2]);
            frustum.planes[5].d = vp[3][3] - vp[3][2];
            for (int pi = 0; pi < 6; pi++) {
                float len = glm::length(frustum.planes[pi].normal);
                if (len > 0.0f) {
                    frustum.planes[pi].normal /= len;
                    frustum.planes[pi].d /= len;
                }
            }
        }

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
            glUniform1i(glGetUniformLocation(program.programId_, "shadowCubeMap"), 2);

            if (isOmni) {
                GLuint shadowCube = (shadowMapIndex < depthCubemaps_.size()) ? depthCubemaps_[shadowMapIndex] : 0;
                glBindTextureUnit(2, shadowCube);
            } else {
                GLuint shadowTex = (shadowMapIndex < depthMaps_.size()) ? depthMaps_[shadowMapIndex] : 0;
                glBindTextureUnit(1, shadowTex);
            }

            int camGridX = -1, camGridZ = -1;

            for (size_t i = 0; i < render->objitem_->meshes.size(); i++) {
                Mesh* mesh = render->objitem_->meshes[i].get();

                if (mesh->name_.size() > 8 && mesh->name_.substr(0, 8) == "terrain_") {
                    int gx, gz, nc, lodLevel;
                    float cx, cz;
                    if (sscanf(mesh->name_.c_str(), "terrain_%d_%d_nc%d_lod%d_wx%f_wz%f",
                        &gx, &gz, &nc, &lodLevel, &cx, &cz) == 6) {
                        if (camGridX == -1) {
                            float denomX = (float)gx + 0.5f - (float)nc * 0.5f;
                            float chunkSizeX = (std::abs(denomX) > 0.001f) ? (cx / denomX) : 1.0f;
                            float denomZ = (float)gz + 0.5f - (float)nc * 0.5f;
                            float chunkSizeZ = (std::abs(denomZ) > 0.001f) ? (cz / denomZ) : 1.0f;
                            float halfWidth = chunkSizeX * nc * 0.5f;
                            float halfDepth = chunkSizeZ * nc * 0.5f;
                            glm::vec3 localCamPos = (viewPos_ - transform->position) / transform->scale;
                            camGridX = glm::clamp((int)std::floor((localCamPos.x + halfWidth) / chunkSizeX), 0, nc - 1);
                            camGridZ = glm::clamp((int)std::floor((localCamPos.z + halfDepth) / chunkSizeZ), 0, nc - 1);
                        }

                        int dx = std::abs(gx - camGridX);
                        int dz = std::abs(gz - camGridZ);
                        int ringDist = dx > dz ? dx : dz;

                        int desiredLod;
                        if (ringDist <= 3) desiredLod = 0;
                        else if (ringDist <= 7) desiredLod = 1;
                        else if (ringDist <= 11) desiredLod = 2;
                        else desiredLod = 3;

                        if (lodLevel != desiredLod) continue;
                    }
                }

                if (mesh->aabbMin != mesh->aabbMax) {
                    glm::vec3 sMin = transform->scale * mesh->aabbMin;
                    glm::vec3 sMax = transform->scale * mesh->aabbMax;
                    glm::vec3 worldAabbMin = transform->position + glm::min(sMin, sMax);
                    glm::vec3 worldAabbMax = transform->position + glm::max(sMin, sMax);
                    if (!IsAABBInFrustum(frustum, worldAabbMin, worldAabbMax))
                        continue;
                }

                if (mesh->materialId_ != -1) {
                    Material mat = render->objitem_->materials.at(mesh->materialId_);
                    if (!mat.loadeable) continue;

                    glBindTextureUnit(0, mat.diffuseTexID);
                    glUniform1i((GLint)loc, 0);

                    glUniform3f(glGetUniformLocation(program.programId_, "DIFFUSE"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "SPECULAR"), mat.specular.x, mat.specular.y, mat.specular.z);

                    glUniform1i(glGetUniformLocation(program.programId_, "useHeightLUT"), mat.useHeightLUT);
                    glUniform1f(glGetUniformLocation(program.programId_, "maxHeight"), mat.maxHeight);
                }

                if (mesh->vao == GL_INVALID_INDEX || mesh->vao == 0) {
                    mesh->GenerateVao();
                    mesh->SetVertexAtribs(attributes);
                }
                glBindVertexArray(mesh->vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));
            }
        }
    }

    void RenderLightsSystem::Render(ECSManager& ecs, glm::mat4x4& model, bool hasShadows) {
        glViewport(0, 0, windowWidth_, windowHeight_);
        glUseProgram(program.programId_);
        glEnable(GL_DEPTH_TEST);

        size_t loc = glGetUniformLocation(program.programId_, "diffuseTexture");
        program.SetupAtributeLocations(attributes);

        auto lightEntities = ecs.GetEntitiesWithComponents<LightComponent, TransformComponent>();
        LightComponent* light = nullptr;
        if (!lightEntities.empty()) {
            light = ecs.GetComponent<LightComponent>(lightEntities[0]);
        }

        glUniform3f(glGetUniformLocation(program.programId_, "viewPos"), viewPos_.x, viewPos_.y, viewPos_.z);
        glUniform1f(glGetUniformLocation(program.programId_, "shininess"), shininess);
        glUniform1f(glGetUniformLocation(program.programId_, "far_plane"), far_plane);
        glUniform1i(glGetUniformLocation(program.programId_, "hasShadows"), hasShadows);

        auto renderables = ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>();

        if (!renderables.empty()) {
            glDepthMask(GL_TRUE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 1);
            if (light && light->hasAmbient_) {
                glUniform3f(glGetUniformLocation(program.programId_, "ambientColor"), light->ambient_.color_.x, light->ambient_.color_.y, light->ambient_.color_.z);
                glUniform1f(glGetUniformLocation(program.programId_, "ambientIntensity"), light->ambient_.intensity_);
            }
            glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 0);
            DrawCall(ecs, model, loc, renderables, 0, false);

            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);
            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 0);

            size_t current2DShadowIndex = 0;
            size_t currentCubeShadowIndex = 0;

            for (size_t light_id : lightEntities) {
                LightComponent* lightComp = ecs.GetComponent<LightComponent>(light_id);

                for (auto& dirLight : lightComp->directionalLights) {
                    glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 1);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightDirOrPos"), dirLight.direction_.x, dirLight.direction_.y, dirLight.direction_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightColor"), dirLight.color_.x, dirLight.color_.y, dirLight.color_.z);
                    glUniform1f(glGetUniformLocation(program.programId_, "lightIntensity"), dirLight.intensity_);
                    lightSpaceMatrix_ = dirLight.getLightSpaceMatrix();
                    glUniformMatrix4fv(glGetUniformLocation(program.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix_));

                    DrawCall(ecs, model, loc, renderables, current2DShadowIndex, false);
                    current2DShadowIndex++;
                }

                for (auto& spot : lightComp->spotLights) {
                    glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 2);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightDirOrPos"), spot.position_.x, spot.position_.y, spot.position_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "spotLightDir"), spot.direction_.x, spot.direction_.y, spot.direction_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightColor"), spot.color_.x, spot.color_.y, spot.color_.z);
                    glUniform1f(glGetUniformLocation(program.programId_, "lightIntensity"), spot.intensity_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotCutOff"), spot.cutOff_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotOuterCutOff"), spot.outerCutOff_);
                    lightSpaceMatrix_ = spot.getLightSpaceMatrix();
                    glUniformMatrix4fv(glGetUniformLocation(program.programId_, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix_));

                    DrawCall(ecs, model, loc, renderables, current2DShadowIndex, false);
                    current2DShadowIndex++;
                }

                for (auto& point : lightComp->pointLights) {
                    glUniform1i(glGetUniformLocation(program.programId_, "lightType"), 3);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightDirOrPos"), point.position_.x, point.position_.y, point.position_.z);
                    glUniform3f(glGetUniformLocation(program.programId_, "lightColor"), point.color_.x, point.color_.y, point.color_.z);
                    glUniform1f(glGetUniformLocation(program.programId_, "lightIntensity"), point.intensity_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotConstant"), point.constant_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotLinear"), point.linear_);
                    glUniform1f(glGetUniformLocation(program.programId_, "spotQuadratic"), point.quadratic_);

                    DrawCall(ecs, model, loc, renderables, currentCubeShadowIndex, true);
                    currentCubeShadowIndex++;
                }
            }

            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }
    }
}
