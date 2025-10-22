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

	struct Material {
		std::string name;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		float shininess;
	};

	class ObjLoader {
	public:
		static std::optional<ObjLoader> loadObj(
			const std::string& filepath,
			const std::string& mtlpath = ""
		);

		const std::vector<Vertex>& getVertices() const { return vertices; }
		const std::vector<Material>& getMaterials() const { return materials; }

	private:
		std::vector<Vertex> vertices;
		std::vector<Material> materials;
	};

}
