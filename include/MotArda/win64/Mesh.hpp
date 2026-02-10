#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"


namespace MTRD {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		Vertex();
		Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm);
	};


	class Mesh {
	public:
		int materialId;
		GLuint vao;
		GLuint gluintVertexBuffer;
		bool debug_;

		int meshSize;

		Mesh(std::vector<Vertex> vertices, bool debug = true);
		~Mesh();
	};

}