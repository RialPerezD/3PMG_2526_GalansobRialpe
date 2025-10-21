#pragma once

#include <optional>
#include <vector>
#include <string>
#include "../deps/glm-master/glm/glm.hpp"

namespace MTRD {

	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	class ObjLoader {
	public:
		static std::optional<ObjLoader> loadObj(const std::string& filepath);

		const std::vector<Vertex>& getVertices() const { return vertices; }

	private:
		std::vector<Vertex> vertices;
	};

}
