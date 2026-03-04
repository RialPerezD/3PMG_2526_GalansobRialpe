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
	class RenderDefferredSystem {
	public:
		RenderDefferredSystem();
	};
}