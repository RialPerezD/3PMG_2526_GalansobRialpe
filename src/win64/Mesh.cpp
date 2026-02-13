#include "../include/MotArda/win64/Mesh.hpp"
#include "../include/MotArda/win64/Debug.hpp"
#include <iostream>


namespace MTRD {
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