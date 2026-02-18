#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/common/Vertex.hpp>

namespace MTRD {
    class Window;

    class Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, Window& window, std::string name, bool& firstTime, int materialId, bool debug);
        ~Mesh();
        Mesh(Mesh&& other);
        Mesh& operator=(Mesh&& other);
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        GLuint vao;
        GLuint gluintVertexBuffer;
        int materialId_;
        int meshSize;
        bool debug_;
        std::string name_;
    };
}