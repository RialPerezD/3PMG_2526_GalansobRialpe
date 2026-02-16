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

		void SetupUniforms(
			std::vector<Window::UniformAttrib>& uniforms,
			const std::vector<Window::VertexAttrib>& attributes,
			bool debug
		);
	private:
		Program(GLuint programId) : programId_{ programId } {
		}

		friend class RenderSystem;
		GLuint programId_;
	};
}