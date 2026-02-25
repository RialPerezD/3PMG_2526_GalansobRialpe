#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Debug.hpp>

namespace MTRD {
    ShadowMapSystem::ShadowMapSystem(glm::mat4& model)
        : shadowProgram{
            Shader::VertexFromFile("../assets/shaders/shadow_map_vertex.txt", true),
            Shader::FragmentFromFile("../assets/shaders/shadow_map_fragment.txt", true), true }
    {
        debug_ = true;

        glGenFramebuffers(1, &depthMapFBO_);

        glGenTextures(1, &depthMap_);
        glBindTexture(GL_TEXTURE_2D, depthMap_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (debug_) {
            glCheckError();
        }

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


    void ShadowMapSystem::DrawCall(ECSManager& ecs, glm::mat4& model) {
        for (size_t id : ecs.GetEntitiesWithComponents<RenderComponent, TransformComponent>()) {
            RenderComponent* render = ecs.GetComponent<RenderComponent>(id);
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(id);

            model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate(model, transform->angleRotationRadians, transform->rotation);
            }
            shadowProgram.SetupUniforms(uniforms);

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


    void ShadowMapSystem::RenderShadowMap(ECSManager& ecs, glm::mat4& model) {

        glUseProgram(shadowProgram.programId_);
        glEnable(GL_DEPTH_TEST);

        shadowProgram.SetupAtributeLocations(attributes);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_BLEND);


        for (size_t light_id : ecs.GetEntitiesWithComponents<LightComponent>()) {
            LightComponent* light = ecs.GetComponent<LightComponent>(light_id);

            for (int i = 0; i < light->directionalLights.size(); i++) {
                lightSpaceMatrix_ = light->directionalLights[i].getLightSpaceMatrix();
                DrawCall(ecs, model);
            }

            for (int i = 0; i < light->spotLights.size(); i++) {
                lightSpaceMatrix_ = light->spotLights[i].getLightSpaceMatrix();
                DrawCall(ecs, model);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, 800, 600);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap_);
        glEnable(GL_BLEND);


        if (debug_) {
            glCheckError();
        }
    }
}