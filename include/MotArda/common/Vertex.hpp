#pragma once
#include "../deps/glm-master/glm/glm.hpp"


namespace MTRD {
	class Vertex {
	public:
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		Vertex();
		Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm);
	};
}