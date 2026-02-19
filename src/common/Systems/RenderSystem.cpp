#include "MotArda/common/Systems/RenderSystem.hpp"
#include <MotArda/win64/Debug.hpp>


namespace MTRD {
	RenderSystem::RenderSystem()
		: program{
			Shader::VertexFromFile("../assets/shaders/textured_obj_vertex.txt",true) ,
			Shader::FragmentFromFile("../assets/shaders/textured_obj_fragment.txt", true),true }
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


	void RenderSystem::Render(
        ECSManager& ecs,
        std::vector<size_t> renderables,
        bool debug
	) {
        glUseProgram(program.programId_);
        auto loc = glGetUniformLocation(program.programId_, "diffuseTexture");
        program.SetupAtributeLocations(attributes);
        program.SetupUniforms(uniforms);

        for (size_t id : renderables) {
            RenderComponent* render = ecs.GetComponent<RenderComponent>(id);
            for (size_t i = 0; i < render->meshes_->size(); i++) {
                Mesh* mesh = render->meshes_->at(i).get();

                if (mesh->materialId_ != -1) {
                    Material mat = render->materials_->at(mesh->materialId_);

                    if (!mat.loadeable) continue;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, mat.diffuseTexID);
                    glUniform1i(loc, 0);

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