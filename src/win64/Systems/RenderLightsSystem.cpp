#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Debug.hpp>


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

        auto lightEntity = ecs.AddEntity();
        LightComponent* light = ecs.AddComponent<LightComponent>(lightEntity);
        light->hasAmbient = true;
        light->ambient = AmbientLight(glm::vec3(0.2f, 0.2f, 0.2f), 0.3f);

        light->hasDirectional = true;
        light->directional = DirectionalLight(glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

        light->hasSpot = true;
        light->spot = SpotLight(
            glm::vec3(0.0f, 5.0f, 5.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.0f,
            glm::cos(glm::radians(12.5f)),
            glm::cos(glm::radians(17.5f)),
            1.0f,
            0.09f,
            0.032f
        );

        glUniform3f(glGetUniformLocation(program.programId_, "viewPos"), viewPos_.x, viewPos_.y, viewPos_.z);
        glUniform1f(glGetUniformLocation(program.programId_, "shininess"), shininess);

        glUniform1i(glGetUniformLocation(program.programId_, "useAmbient"), light->hasAmbient ? 1 : 0);
        if (light->hasAmbient) {
            glUniform3f(glGetUniformLocation(program.programId_, "ambientColor"), light->ambient.color.x, light->ambient.color.y, light->ambient.color.z);
            glUniform1f(glGetUniformLocation(program.programId_, "ambientIntensity"), light->ambient.intensity);
        }

        glUniform1i(glGetUniformLocation(program.programId_, "useDirectional"), light->hasDirectional ? 1 : 0);
        if (light->hasDirectional) {
            glUniform3f(glGetUniformLocation(program.programId_, "directionalDir"), light->directional.direction.x, light->directional.direction.y, light->directional.direction.z);
            glUniform3f(glGetUniformLocation(program.programId_, "directionalColor"), light->directional.color.x, light->directional.color.y, light->directional.color.z);
            glUniform1f(glGetUniformLocation(program.programId_, "directionalIntensity"), light->directional.intensity);
        }

        glUniform1i(glGetUniformLocation(program.programId_, "useSpot"), light->hasSpot ? 1 : 0);
        if (light->hasSpot) {
            glUniform3f(glGetUniformLocation(program.programId_, "spotPos"), light->spot.position.x, light->spot.position.y, light->spot.position.z);
            glUniform3f(glGetUniformLocation(program.programId_, "spotDir"), light->spot.direction.x, light->spot.direction.y, light->spot.direction.z);
            glUniform3f(glGetUniformLocation(program.programId_, "spotColor"), light->spot.color.x, light->spot.color.y, light->spot.color.z);
            glUniform1f(glGetUniformLocation(program.programId_, "spotIntensity"), light->spot.intensity);
            glUniform1f(glGetUniformLocation(program.programId_, "spotCutOff"), light->spot.cutOff);
            glUniform1f(glGetUniformLocation(program.programId_, "spotOuterCutOff"), light->spot.outerCutOff);
            glUniform1f(glGetUniformLocation(program.programId_, "spotConstant"), light->spot.constant);
            glUniform1f(glGetUniformLocation(program.programId_, "spotLinear"), light->spot.linear);
            glUniform1f(glGetUniformLocation(program.programId_, "spotQuadratic"), light->spot.quadratic);
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
