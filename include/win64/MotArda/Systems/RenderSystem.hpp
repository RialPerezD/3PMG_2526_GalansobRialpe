#pragma once
#include <MotArda/Shader.hpp>
#include <MotArda/Program.hpp>
#include <MotArda/Components/TransformComponent.hpp>
#include <MotArda/Components/RenderComponent.hpp>
#include <MotArda/Ecs.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace MTRD {
	class RenderSystem {
	public:
		RenderSystem(glm::mat4x4& vp, glm::mat4x4& model, bool& debug);

		void Render(
			ECSManager& ecs,
			glm::mat4x4& model
		);

		std::vector<VertexAttribute> attributes;
		std::vector<Window::UniformAttrib> uniforms;
	private:
		bool debug_ = true;
		Program program;
	};
}