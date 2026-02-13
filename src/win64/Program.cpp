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


    void Program::SetupUniforms(
        std::vector<Window::UniformAttrib>& uniforms,
        const std::vector<Window::VertexAttrib>& attributes,
        bool debug
    ) {
        // Uniform locations
        for (auto& u : uniforms) {
            u.location = glGetUniformLocation(programId_, u.name);
        }

        // Attribute locations
        for (auto& attr : attributes) {
            GLint loc = glGetAttribLocation(programId_, attr.name);
            if (loc >= 0) {
                glEnableVertexAttribArray(loc);
                glVertexAttribPointer(loc, attr.size, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)attr.offset);
            }
        }

        if (debug) {
            glCheckError();
        }
    }
}