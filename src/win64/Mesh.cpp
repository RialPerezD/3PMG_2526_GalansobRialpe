#include "../include/MotArda/win64/Mesh.hpp"
#include "../include/MotArda/win64/Debug.hpp"
#include <MotArda/win64/window.hpp>
#include <iostream>
#include <memory>

namespace MTRD {

    Mesh::Mesh(
        std::vector<Vertex> vertices,
        Window& window,
        std::string name,
        bool& firstTime,
        int materialId,
        bool debug)
    {
        name_ = name;

        if (firstTime) {
            glfwMakeContextCurrent(window.getGlfwSecondaryWindow());
            firstTime = false;
        }

        meshSize = static_cast<int>(vertices.size());
        vao = GL_INVALID_INDEX;
        gluintVertexBuffer = GL_INVALID_INDEX;
        materialId_ = materialId;

        const void* vertex = static_cast<const void*> (vertices.data());

        glGenBuffers(1, &gluintVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gluintVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshSize, vertices.data(), GL_STATIC_DRAW);

        debug_ = debug;
        vertices.clear();
		glFlush();

        if (debug_) {
            glCheckError();
        }
    }


    Mesh::~Mesh() {
        if (vao != GL_INVALID_INDEX && vao != 0) {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &gluintVertexBuffer);
            glDeleteVertexArrays(1, &vao);
        }

        if (debug_) {
            glCheckError();
        }
    }


    Mesh::Mesh(Mesh&& other)
        : vao(other.vao),
        gluintVertexBuffer(other.gluintVertexBuffer),
        materialId_(other.materialId_),
        debug_(other.debug_),
        meshSize(other.meshSize),
        name_(std::move(other.name_))
    {
        other.vao = 0;
        other.gluintVertexBuffer = 0;
    }


    Mesh& Mesh::operator=(Mesh&& other) {
        if (this != &other) {
            if (vao != 0 && vao != GL_INVALID_INDEX) {
                glDeleteBuffers(1, &gluintVertexBuffer);
                glDeleteVertexArrays(1, &vao);
            }
            vao = other.vao;
            gluintVertexBuffer = other.gluintVertexBuffer;
            materialId_ = other.materialId_;
            debug_ = other.debug_;
            meshSize = other.meshSize;
            name_ = std::move(other.name_);

            other.vao = 0;
            other.gluintVertexBuffer = 0;
        }
        return *this;
    }


    void Mesh::GenerateVao() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        if (debug_) {
            glCheckError();
        }
    }


    void Mesh::SetVertexAtribs(const std::vector<VertexAttribute>& attributes) {
        glBindBuffer(GL_ARRAY_BUFFER, gluintVertexBuffer);

        for (int i = 0; i < attributes.size(); i++) {
            if (attributes[i].location < 0) continue;

            glEnableVertexAttribArray(attributes[i].location);
            glVertexAttribPointer(
                attributes[i].location,
                attributes[i].size,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)attributes[i].offset
            );

            if (debug_) {
                glCheckError();
            }
        }
    }
}