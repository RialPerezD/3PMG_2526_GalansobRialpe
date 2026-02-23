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
        };
    }


    void RenderLightsSystem::Render(
        ECSManager& ecs,
        std::vector<size_t> renderables,
        glm::mat4x4& model,
        bool debug
    ) {
        glUseProgram(program.programId_);
        auto loc = glGetUniformLocation(program.programId_, "diffuseTexture");
        program.SetupAtributeLocations(attributes);

        auto lightEntities = ecs.GetEntitiesWithComponents<LightComponent>();
        LightComponent* light = nullptr;
        if (!lightEntities.empty()) {
            light = ecs.GetComponent<LightComponent>(lightEntities[0]);
        }

        glUniform3f(glGetUniformLocation(program.programId_, "viewPos"), viewPos_.x, viewPos_.y, viewPos_.z);
        glUniform1f(glGetUniformLocation(program.programId_, "shininess"), shininess);

        if (light && light->hasAmbient) {
            glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), 1);
            glUniform3f(glGetUniformLocation(program.programId_, "ambientColor"), light->ambient.color.x, light->ambient.color.y, light->ambient.color.z);
            glUniform1f(glGetUniformLocation(program.programId_, "ambientIntensity"), light->ambient.intensity);
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
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);

            prefix = "directionalColor[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), dirLight.color.x, dirLight.color.y, dirLight.color.z);

            prefix = "directionalIntensity[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), dirLight.intensity);
        }

        int numSpotLights = 0;
        if (light) {
            numSpotLights = static_cast<int>(light->spotLights.size());
        }
        glUniform1i(glGetUniformLocation(program.programId_, "numSpotLights"), numSpotLights);

        for (size_t i = 0; i < 4 && light && i < light->spotLights.size(); i++) {
            const auto& spot = light->spotLights[i];
            std::string prefix = "spotPos[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.position.x, spot.position.y, spot.position.z);

            prefix = "spotDir[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.direction.x, spot.direction.y, spot.direction.z);

            prefix = "spotColor[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.color.x, spot.color.y, spot.color.z);

            prefix = "spotIntensity[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.intensity);

            prefix = "spotCutOff[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.cutOff);

            prefix = "spotOuterCutOff[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.outerCutOff);

            prefix = "spotConstant[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.constant);

            prefix = "spotLinear[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.linear);

            prefix = "spotQuadratic[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(program.programId_, prefix.c_str()), spot.quadratic);
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

                    if (debug) {
                        glCheckError();
                    }
                }

                if (mesh->vao == GL_INVALID_INDEX || mesh->vao == 0) {
                    mesh->GenerateVao();
                    mesh->SetVertexAtribs(attributes);
                }
                glBindVertexArray(mesh->vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));

                if (debug) {
                    glCheckError();
                }
            }
        }
    }
}
