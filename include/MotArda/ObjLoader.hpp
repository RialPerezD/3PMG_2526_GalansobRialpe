#pragma once

#include <optional>
#include <vector>
#include <string>
#include "glad/glad.h"
#include "../deps/glm-master/glm/glm.hpp"
#include "Components/Render.hpp"

namespace MTRD {
	class ObjLoader {
	public:
		static std::optional<ObjLoader> loadObj(
			const std::string& filepath
		);

		const std::vector<Shape>& getShapes() const { return shapes; }
		const std::vector<Material>& getMaterials() const { return materials; }

	private:
		std::vector<Shape> shapes;
		std::vector<Material> materials;
	};

}
