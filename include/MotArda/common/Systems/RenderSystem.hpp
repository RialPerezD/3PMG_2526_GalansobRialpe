#pragma once
#include <MotArda/win64/Shader.hpp>
#include <MotArda/win64/Program.hpp>
#include <MotArda/common/Components/TransformComponent.hpp>
#include <MotArda/common/Components/RenderComponent.hpp>
#include <MotArda/common/Ecs.hpp>


namespace MTRD {
	class RenderSystem {
	public:
		RenderSystem();

		void Render(
			ECSManager& ecs,
			std::vector<size_t> renderables,
			bool debug
		);

	private:
		Program program;
	};
}