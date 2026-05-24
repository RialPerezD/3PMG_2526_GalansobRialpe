#include <MotArda/Debug.hpp>
#include <MotArda/Mesh.hpp>
#include <MotArda/window.hpp>
#include <iostream>
#include <memory>

namespace MTRD {

    Mesh::Mesh(
        const std::vector<Vertex>& vertices,
        Window& window,
        const std::string& name,
        bool& firstTime,
        int materialId,
        bool debug)
        : name_(name),
        meshSize(static_cast<int>(vertices.size())),
        vao(GL_INVALID_INDEX),
        gluintVertexBuffer(GL_INVALID_INDEX),
        materialId_(materialId),
        debug_(debug) {

        (void)window;
        (void)firstTime;

        glGenBuffers(1, &gluintVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gluintVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshSize, vertices.data(), GL_STATIC_DRAW);

        if (debug_) {
            glCheckError();
        }
    }


    Mesh::~Mesh() {
        if (vao != GL_INVALID_INDEX && vao != 0) {
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
        name_(std::move(other.name_)) {
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


    void Mesh::RemoveContext() {
    }

    void Mesh::GenerateVao() {
        glGenVertexArrays(1, &vao);

        if (debug_) {
            glCheckError();
        }
    }


    void Mesh::SetVertexAtribs(const std::vector<VertexAttribute>& attributes) {
        glBindVertexArray(vao);
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
                reinterpret_cast<void*>(static_cast<uintptr_t>(attributes[i].offset))
            );

            if (debug_) {
                glCheckError();
            }
        }
    }


}