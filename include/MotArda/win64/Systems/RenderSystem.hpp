#pragma once
#include <MotArda/win64/Shader.hpp>
#include <MotArda/win64/Program.hpp>
#include <MotArda/common/Components/TransformComponent.hpp>
#include <MotArda/common/Components/RenderComponent.hpp>
#include <MotArda/common/Ecs.hpp>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"


namespace MTRD {
	class RenderSystem {
	public:
		RenderSystem(glm::mat4x4& vp, glm::mat4x4& model);

		void Render(
			ECSManager& ecs,
			std::vector<size_t> renderables,
			glm::mat4x4& model,
			bool debug
		);

		std::vector<VertexAttribute> attributes;
		std::vector<Window::UniformAttrib> uniforms;
	private:
		Program program;
	};
}