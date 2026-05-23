#include <MotArda/Shader.hpp>
#include <MotArda/Debug.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace MTRD {

    Shader Shader::VertexFromFile(std::filesystem::path filename, bool debug) {
        { std::ofstream file("testeo.txt", std::ios::app); file << "dentro shader carga\n"; }
        std::ifstream file(filename);
        if (!file.is_open()) {
            { std::ofstream file("testeo.txt", std::ios::app); file << "No se pudo abrir el archivo " << filename.string()<<"\n"; }
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
                { std::ofstream file("testeo.txt", std::ios::app); file << "ERROR::SHADER::VERTEX::LINKING_FAILED "<< infoLog<<"\n"; }
            } else {
                { std::ofstream file("testeo.txt", std::ios::app); file << "Vertex "<< filename.filename().string().c_str()<<" shader compiled successfully\n"; }
            }
        }

        if (debug) {
            glCheckError();
        }

        { std::ofstream file("testeo.txt", std::ios::app); file << "fin shader carga\n"; }
        return Shader{ vertexShader };
    }


    Shader Shader::GeometryFromFile(std::filesystem::path filename, bool debug) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo: " + filename.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string shaderSource = buffer.str();
        const char* text = shaderSource.c_str();

        GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &text, nullptr);
        glCompileShader(geometryShader);

        if (debug) {
            GLint success;
            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(geometryShader, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n%s\n", infoLog);   
                fflush(stderr);
            } else {
                fprintf(stderr, "Geometry %s shader compiled successfully.\n", filename.filename().string().c_str());
                fflush(stderr);
            }
            glCheckError();
        }
        return Shader{ geometryShader };
    }


    Shader Shader::FragmentFromFile(std::filesystem::path filename, bool debug) {
        { std::ofstream file("testeo.txt", std::ios::app); file << "dentro fragment carga\n"; }
        std::ifstream file(filename);
        if (!file.is_open()) {
            { std::ofstream file("testeo.txt", std::ios::app); file << "No se pudo abrir el archivo " << filename.string() << "\n"; }
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
                { std::ofstream file("testeo.txt", std::ios::app); file << "ERROR::SHADER::VERTEX::LINKING_FAILED " << infoLog << "\n"; }
            } else {
                { std::ofstream file("testeo.txt", std::ios::app); file << "Vertex " << filename.filename().string().c_str() << " shader compiled successfully\n"; }
            }
        }

        if (debug) {
            glCheckError();
        }
        { std::ofstream file("testeo.txt", std::ios::app); file << "fuera fragment carga\n"; }
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