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

        if (firstTime) {
            glfwMakeContextCurrent(window.getGlfwSecondaryWindow());

            if (debug_) {
                glCheckError();
            }

            firstTime = false;
        }

        glCreateBuffers(1, &gluintVertexBuffer);
        glNamedBufferData(gluintVertexBuffer, sizeof(Vertex) * meshSize, vertices.data(), GL_STATIC_DRAW);

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


    Mesh::Mesh(Mesh&& other) noexcept
        : vao(other.vao),
        gluintVertexBuffer(other.gluintVertexBuffer),
        materialId_(other.materialId_),
        debug_(other.debug_),
        meshSize(other.meshSize),
        name_(std::move(other.name_)),
        aabbMin(other.aabbMin),
        aabbMax(other.aabbMax) {
        other.vao = 0;
        other.gluintVertexBuffer = 0;
        other.aabbMin = glm::vec3(0.0f);
        other.aabbMax = glm::vec3(0.0f);
    }


    Mesh& Mesh::operator=(Mesh&& other) noexcept {
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
            aabbMin = other.aabbMin;
            aabbMax = other.aabbMax;

            other.vao = 0;
            other.gluintVertexBuffer = 0;
            other.aabbMin = glm::vec3(0.0f);
            other.aabbMax = glm::vec3(0.0f);
        }
        return *this;
    }


    void Mesh::RemoveContext() {
        glfwMakeContextCurrent(NULL);
    }

    void Mesh::GenerateVao() {
        glCreateVertexArrays(1, &vao);

        if (debug_) {
            glCheckError();
        }
    }


    void Mesh::SetVertexAtribs(const std::vector<VertexAttribute>& attributes) {
        const GLuint bindingIndex = 0;
        glVertexArrayVertexBuffer(vao, bindingIndex, gluintVertexBuffer, 0, sizeof(Vertex));

        for (int i = 0; i < attributes.size(); i++) {
            if (attributes[i].location < 0) continue;

            glEnableVertexArrayAttrib(vao, attributes[i].location);
            glVertexArrayAttribFormat(
                vao,
                attributes[i].location,
                attributes[i].size,
                GL_FLOAT,
                GL_FALSE,
                static_cast<GLuint>(attributes[i].offset)
            );
            glVertexArrayAttribBinding(vao, attributes[i].location, bindingIndex);

            if (debug_) {
                glCheckError();
            }
        }
    }


}