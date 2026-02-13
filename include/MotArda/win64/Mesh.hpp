#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/common/Vertex.hpp>


namespace MTRD {
	class Mesh {
	public:
		int materialId_;
		GLuint vao;
		GLuint gluintVertexBuffer;
		bool debug_;

		int meshSize;

		Mesh(std::vector<Vertex> vertices, int materialId = -1, bool debug = true);
		~Mesh();
	};

}