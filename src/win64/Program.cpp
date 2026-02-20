#include "../include/MotArda/win64/Program.hpp"
#include <MotArda/win64/Debug.hpp>
#include <stdio.h>


namespace MTRD {
    Program::Program(const Shader& vertexShader, const Shader& fragmentShader, bool debug) {
        programId_ = glCreateProgram();
        glAttachShader(programId_, vertexShader.id_);
        glAttachShader(programId_, fragmentShader.id_);
        glLinkProgram(programId_);

        if (debug) {
            GLint success;
            glGetProgramiv(programId_, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(programId_, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
            }
            else {
                printf("Program linked successfully.\n");
            }
        }

        glUseProgram(programId_);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (debug) {
            glCheckError();
        }
    }


	Program::~Program() {
        if (programId_ != 0) {
            glDeleteProgram(programId_);
        }
	}


    Program::Program(Program&& other) {
        other.programId_ = 0;
    }


    Program& Program::operator=(Program&& other) {
        if (this != &other) {
            if (programId_ != 0) {
                glDeleteShader(programId_);
            }

            programId_ = other.programId_;
            other.programId_ = 0;
        }

        return *this;
    }


    void Program::SetupAtributeLocations(
        std::vector<VertexAttribute>& attributes,
        bool debug
    ) {
        for (auto& attr : attributes) {
            GLint loc = glGetAttribLocation(programId_, attr.name);
            if (loc >= 0) {
                attr.location = loc;
            }
        }

        if (debug) {
            glCheckError();
        }
    }


    void Program::SetupUniforms(
        std::vector<Window::UniformAttrib>& uniforms,
        bool debug
    ){
        for (Window::UniformAttrib& uniform : uniforms) {

            uniform.location = glGetUniformLocation(programId_, uniform.name);

            if (uniform.location < 0) continue;

            switch (uniform.type) {
            case Window::UniformTypes::Vec2:
                glUniform2fv(uniform.location, 1, uniform.values);
                break;
            case Window::UniformTypes::Vec3:
                glUniform3fv(uniform.location, 1, uniform.values);
                break;
            case Window::UniformTypes::Vec4:
                glUniform4fv(uniform.location, 1, uniform.values);
                break;
            case Window::UniformTypes::Mat2:
                glUniformMatrix2fv(uniform.location, 1, GL_FALSE, uniform.values);
                break;
            case Window::UniformTypes::Mat3:
                glUniformMatrix3fv(uniform.location, 1, GL_FALSE, uniform.values);
                break;
            case Window::UniformTypes::Mat4:
                glUniformMatrix4fv(uniform.location, 1, GL_FALSE, uniform.values);
                break;
            default:
                break;
            }

            if (debug) {
                glCheckError();
            }
        }
    }
}