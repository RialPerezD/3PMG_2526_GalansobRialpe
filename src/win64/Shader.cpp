#include "Motarda/win64/Shader.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <MotArda/win64/Debug.hpp>

namespace MTRD {

    Shader Shader::VertexFromFile(std::filesystem::path filename, bool debug) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo: " + filename.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string shaderSource = buffer.str();
        const char* text = shaderSource.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &text, nullptr);
        glCompileShader(vertexShader);

        if (debug) {
            GLint success;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
            }
            else {
                printf("Vertex %s shader compiled successfully.\n", filename.filename().string().c_str());
            }
        }

        if (debug) {
            glCheckError();
        }
        return Shader{ vertexShader };
    }


    Shader Shader::FragmentFromFile(std::filesystem::path filename, bool debug) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo: " + filename.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string fragmentSource = buffer.str();
        const char* text = fragmentSource.c_str();

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &text, nullptr);
        glCompileShader(fragmentShader);
        
        if (debug) {
            GLint success;
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
            }
            else {
                printf("Fragment %s shader compiled successfully.\n", filename.filename().string().c_str());
            }
        }

        if (debug) {
            glCheckError();
        }
        return Shader{ fragmentShader };
    }


    Shader::~Shader() {
        if (id_ != 0) {
            glDeleteShader(id_);
        }
    }


    Shader::Shader(Shader&& other) : id_{ other.id_ } {
        other.id_ = 0;
    }


    Shader& Shader::operator=(Shader&& other) {
        if (this != &other) {
            if (id_ != 0) {
                glDeleteShader(id_);
            }

            id_ = other.id_;
            other.id_ = 0;
        }

        return *this;
    }


}