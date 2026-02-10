#include "../include/MotArda/win64/Mesh.hpp"

namespace MTRD {

    Vertex::Vertex() : position(0.0f), uv(0.0f), normal(0.0f) {}


    Vertex::Vertex(
        glm::vec3 pos,
        glm::vec2 uv,
        glm::vec3 norm
    ) : position(pos), uv(uv), normal(norm) {}


    Mesh::Mesh(std::vector<Vertex> vertices, bool debug) {
        materialId = -1;
        meshSize = vertices.size();

        glGenVertexArrays(1, &vao);
        const void* vertex = static_cast<const void*> (vertices.data());

        glBindVertexArray(vao);
        glGenBuffers(1, &gluintVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gluintVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshSize, vertex, GL_STATIC_DRAW);


        debug_ = debug;

        if (debug_) {
            //glCheckError();
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
            //glCheckError();
        }
    }
}