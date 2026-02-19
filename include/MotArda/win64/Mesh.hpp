#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/common/Vertex.hpp>
#include <MotArda/common/VertexAttributes.hpp>

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

        static std::unique_ptr<Mesh> MeshUniquePtr(std::vector<Vertex> vertices, Window& window, std::string name, bool& firstTime, int materialId, bool debug) {
            return std::make_unique<Mesh>(vertices, window, name, firstTime, materialId, debug);
        }

        void GenerateVao();
        void SetVertexAtribs(const std::vector<VertexAttribute>& attributes);

        GLuint vao;
        GLuint gluintVertexBuffer;
        int materialId_;
        int meshSize;
        bool debug_;
        std::string name_;
    };
}