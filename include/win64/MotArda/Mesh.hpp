#pragma once

#include <MotArda/Vertex.hpp>
#include <MotArda/VertexAttributes.hpp>

#include <memory>
#include <string>
#include <vector>
#include "glad/glad.h"

#include "../../deps/glm-master/glm/glm.hpp"

namespace MTRD {
    class Window;

    class Mesh {
    public:
        Mesh(
            const std::vector<Vertex>& vertices,
            Window& window,
            const std::string& name,
            bool& firstTime,
            int materialId,
            bool debug
        );    
        

        ~Mesh();
        Mesh(Mesh&& other);
        Mesh& operator=(Mesh&& other);
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        static std::unique_ptr<Mesh> MeshUniquePtr(std::vector<Vertex> vertices, Window& window, std::string name, bool& firstTime, int materialId, bool debug) {
            return std::make_unique<Mesh>(vertices, window, name, firstTime, materialId, debug);
        }

        static void RemoveContext();

        void GenerateVao();
        void SetVertexAtribs(const std::vector<VertexAttribute>& attributes);

        GLuint vao;
        GLuint gluintVertexBuffer;
        int materialId_;
        int meshSize;
        bool debug_;
        std::string name_;
        glm::vec3 aabbMin = glm::vec3(0.0f);
        glm::vec3 aabbMax = glm::vec3(0.0f);
    };
}