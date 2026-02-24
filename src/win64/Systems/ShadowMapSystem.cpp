#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Debug.hpp>

namespace MTRD {
    ShadowMapSystem::ShadowMapSystem()
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        if (debug_) {
            glCheckError();
        }
    }

    ShadowMapSystem::~ShadowMapSystem() {
    }

    void ShadowMapSystem::RenderShadowMap(ECSManager& ecs, std::vector<size_t> renderables, const glm::mat4& lightSpaceMatrix) {

        glUseProgram(shadowProgram.programId_);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
        glClear(GL_DEPTH_BUFFER_BIT);

        GLuint lightMatrixLoc = glGetUniformLocation(shadowProgram.programId_, "lightSpaceMatrix");
        glUniformMatrix4fv(lightMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        for (size_t id : renderables) {
            RenderComponent* render = ecs.GetComponent<RenderComponent>(id);
            TransformComponent* transform = ecs.GetComponent<TransformComponent>(id);

            glm::mat4 model = glm::mat4(1.f);
            model = glm::translate(model, transform->position);
            model = glm::scale(model, transform->scale);
            if (glm::length(transform->rotation) != 0) {
                model = glm::rotate(model, transform->angleRotationRadians, transform->rotation);
            }

            GLuint modelLoc = glGetUniformLocation(shadowProgram.programId_, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            for (size_t i = 0; i < render->meshes_->size(); i++) {
                Mesh* mesh = render->meshes_->at(i).get();

                // --- NUEVO: Generar VAO si no existe ---
                if (mesh->vao == GL_INVALID_INDEX || mesh->vao == 0) {
                    mesh->GenerateVao();
                    // Importante: El Shadow Shader necesita al menos el atributo de posición (location 0)
                    std::vector<VertexAttribute> shadowAttr = {
                        { "position", 3, offsetof(Vertex, position), -1},
                        { "uv", 2, offsetof(Vertex, uv), -1},
                        { "normal", 3, offsetof(Vertex, normal), -1}
                    };

                    mesh->SetVertexAtribs(shadowAttr);
                }
                // --- ---

                glBindVertexArray(mesh->vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        if (debug_) {
            glCheckError();
        }
    }
}