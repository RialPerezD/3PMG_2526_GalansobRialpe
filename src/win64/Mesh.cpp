#include "../include/MotArda/win64/Mesh.hpp"
#include <iostream>

namespace MTRD {

    static GLenum glCheckError_(const char* file, int line) {
        GLenum errorCode;
        while ((errorCode = glGetError()) != GL_NO_ERROR)
        {
            std::string error;
            switch (errorCode)
            {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << error << " | " << file << " (" << line << ")" << std::endl;
        }
        return errorCode;
    }
    #define glCheckError() glCheckError_(__FILE__, __LINE__) 



    Vertex::Vertex() : position(0.0f), uv(0.0f), normal(0.0f) {}

    Vertex::Vertex(
        glm::vec3 pos,
        glm::vec2 uv,
        glm::vec3 norm
    ) : position(pos), uv(uv), normal(norm) {}


    Mesh::Mesh(std::vector<Vertex> vertices, int materialId, bool debug){
        meshSize = static_cast<int>(vertices.size());
        vao = GL_INVALID_INDEX;
        gluintVertexBuffer = GL_INVALID_INDEX;
        materialId_ = materialId;

        glGenVertexArrays(1, &vao);
        const void* vertex = static_cast<const void*> (vertices.data());

        glBindVertexArray(vao);
        glGenBuffers(1, &gluintVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gluintVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshSize, vertex, GL_STATIC_DRAW);

        debug_ = debug;
        vertices.clear();

        if (debug_) {
            glCheckError();
        }
    }


    Mesh::~Mesh(){
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDeleteBuffers(1, &gluintVertexBuffer);
        gluintVertexBuffer = GL_INVALID_INDEX;
        glDeleteVertexArrays(1, &vao);
        vao = GL_INVALID_INDEX;

        if (debug_) {
            glCheckError();
        }
    }


}