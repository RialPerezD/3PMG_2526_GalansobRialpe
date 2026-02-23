#pragma once
#include <glad/glad.h>
#include "Shader.hpp"
#include "window.hpp"


namespace MTRD {
	class Shader;
	class Program {
	public:
		Program(const Shader& vertexShader, const Shader& fragmentShader, bool debug);
		~Program();

		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

		Program(Program&& other);
		Program& operator=(Program&&);

		void SetupAtributeLocations(
			std::vector<VertexAttribute>& attributes,
			bool debug = true
		);

		void SetupUniforms(
			std::vector<Window::UniformAttrib>& uniforms,
			bool debug = true
		);

		GLuint programId_;

	private:
		Program(GLuint programId) : programId_{ programId } {
		}
	};
}