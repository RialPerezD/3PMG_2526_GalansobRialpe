#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Debug.hpp>
#include <string>


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


    void RenderLightsSystem::DrawCall(ECSManager& ecs, glm::mat4x4& model, size_t loc) {
        for (size_t id : ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>()) {
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

        // --- Lights ---
        auto lightEntities = ecs.GetEntitiesWithComponents<LightComponent>();
        LightComponent* light = nullptr;
        if (!lightEntities.empty()) {
            light = ecs.GetComponent<LightComponent>(lightEntities[0]);
        }

        glUniform3f(glGetUniformLocation(program.programId_, "viewPos"), viewPos_.x, viewPos_.y, viewPos_.z);
        glUniform1f(glGetUniformLocation(program.programId_, "shininess"), shininess);

        if (light && light->hasAmbient_) {
            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 1);
            glUniform3f(glGetUniformLocation(program.programId_, "ambientColor"), light->ambient_.color_.x, light->ambient_.color_.y, light->ambient_.color_.z);
            glUniform1f(glGetUniformLocation(program.programId_, "ambientIntensity"), light->ambient_.intensity_);
        } else {
            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 0);
        }

        int numDirLights = 0;
        if (light) {
            numDirLights = static_cast<int>(light->directionalLights.size());
        }
        glUniform1i(glGetUniformLocation(program.programId_, "numDirectionalLights"), numDirLights);

        for (size_t i = 0; i < 4 && light && i < light->directionalLights.size(); i++) {
            const auto& dirLight = light->directionalLights[i];
            std::string prefix = "directionalDir[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), dirLight.direction_.x, dirLight.direction_.y, dirLight.direction_.z);

            prefix = "directionalColor[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), dirLight.color_.x, dirLight.color_.y, dirLight.color_.z);

            prefix = "directionalIntensity[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), dirLight.intensity_);
        }

        int numSpotLights = 0;
        if (light) {
            numSpotLights = static_cast<int>(light->spotLights.size());
        }
        glUniform1i(glGetUniformLocation(program.programId_, "numSpotLights"), numSpotLights);

        for (size_t i = 0; i < 4 && light && i < light->spotLights.size(); i++) {
            const auto& spot = light->spotLights[i];
            std::string prefix = "spotPos[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.position_.x, spot.position_.y, spot.position_.z);

            prefix = "spotDir[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.direction_.x, spot.direction_.y, spot.direction_.z);

            prefix = "spotColor[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.color_.x, spot.color_.y, spot.color_.z);

            prefix = "spotIntensity[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.intensity_);

            prefix = "spotCutOff[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.cutOff_);

            prefix = "spotOuterCutOff[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.outerCutOff_);

            prefix = "spotConstant[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.constant_);

            prefix = "spotLinear[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.linear_);

            prefix = "spotQuadratic[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.quadratic_);
        }
        // --- *** ---


        // --- Renderables ---
        if (hasShadows) {
            for (size_t light_id : ecs.GetEntitiesWithComponents<LightComponent>()) {
                LightComponent* light = ecs.GetComponent<LightComponent>(light_id);

                for (int i = 0; i < light->directionalLights.size(); i++) {
                    lightSpaceMatrix_ = light->directionalLights[i].getLightSpaceMatrix();
                    DrawCall(ecs, model, loc);
                }

                for (int i = 0; i < light->spotLights.size(); i++) {
                    lightSpaceMatrix_ = light->spotLights[i].getLightSpaceMatrix();
                    DrawCall(ecs, model, loc);
                }
            }
        } else {
            lightSpaceMatrix_ = glm::mat4(0);
            DrawCall(ecs, model, loc);
        }
        // --- *** ---
    }
}
