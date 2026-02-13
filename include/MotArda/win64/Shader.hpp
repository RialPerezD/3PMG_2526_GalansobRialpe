#pragma once
#include <filesystem>
#include <glad/glad.h>

namespace MTRD {
	class Shader {
	public:
		static Shader VertexFromFile(std::filesystem::path filename, bool debug);
		static Shader FragmentFromFile(std::filesystem::path filename, bool debug);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

		Shader(Shader&& other);
		Shader& operator=(Shader&& other);
	private:
		Shader(GLuint id) : id_{ id } {
		}

		friend class Program;
		GLuint id_;
	};
}