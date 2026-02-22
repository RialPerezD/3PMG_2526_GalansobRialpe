#include "MotArda/common/Geometries.hpp"
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>
#include <cmath>
#include <vector>
#include "../../deps/glm-master/glm/ext/scalar_constants.hpp"

namespace MTRD {

    std::vector<Vertex> Geometries::GenerateCube(float size) {
        float h = size * 0.5f;
        
        std::vector<Vertex> vertices = {
            // Front face
            Vertex(glm::vec3(-h, -h, h), glm::vec2(0, 1), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(1, 1), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(h, h, h), glm::vec2(1, 0), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(-h, -h, h), glm::vec2(0, 1), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(h, h, h), glm::vec2(1, 0), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(0, 0), glm::vec3(0, 0, 1)),
            
            // Back face
            Vertex(glm::vec3(h, -h, -h), glm::vec2(0, 1), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(1, 1), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(1, 0), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(h, -h, -h), glm::vec2(0, 1), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(1, 0), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(0, 0), glm::vec3(0, 0, -1)),
            
            // Top face
            Vertex(glm::vec3(-h, h, h), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(h, h, h), glm::vec2(1, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(0, 0), glm::vec3(0, 1, 0)),
            
            // Bottom face
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(h, -h, -h), glm::vec2(1, 1), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(-h, -h, h), glm::vec2(0, 0), glm::vec3(0, -1, 0)),
            
            // Right face
            Vertex(glm::vec3(h, -h, h), glm::vec2(0, 1), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, -h, -h), glm::vec2(1, 1), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(0, 1), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, h, h), glm::vec2(0, 0), glm::vec3(1, 0, 0)),
            
            // Left face
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, -h, h), glm::vec2(1, 1), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(1, 0), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(1, 0), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(0, 0), glm::vec3(-1, 0, 0)),
        };
        
        return vertices;
    }

    std::vector<Vertex> Geometries::GeneratePlane(float width, float height) {
        float w = width * 0.5f;
        float h = height * 0.5f;
        
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-w, 0, h), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(w, 0, h), glm::vec2(1, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(w, 0, -h), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(-w, 0, h), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(w, 0, -h), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(-w, 0, -h), glm::vec2(0, 0), glm::vec3(0, 1, 0)),
        };
        
        return vertices;
    }

    std::vector<Vertex> Geometries::GenerateSphere(float radius, int segments, int rings) {
        std::vector<Vertex> vertices;
        
        for (int ring = 0; ring < rings; ring++) {
            float phi1 = static_cast<float>(ring) * glm::pi<float>() / rings;
            float phi2 = static_cast<float>(ring + 1) * glm::pi<float>() / rings;
            
            for (int seg = 0; seg < segments; seg++) {
                float theta1 = static_cast<float>(seg) * 2.0f * glm::pi<float>() / segments;
                float theta2 = static_cast<float>(seg + 1) * 2.0f * glm::pi<float>() / segments;
                
                glm::vec3 p11 = glm::vec3(
                    radius * sin(phi1) * cos(theta1),
                    radius * cos(phi1),
                    radius * sin(phi1) * sin(theta1)
                );
                glm::vec3 p12 = glm::vec3(
                    radius * sin(phi1) * cos(theta2),
                    radius * cos(phi1),
                    radius * sin(phi1) * sin(theta2)
                );
                glm::vec3 p21 = glm::vec3(
                    radius * sin(phi2) * cos(theta1),
                    radius * cos(phi2),
                    radius * sin(phi2) * sin(theta1)
                );
                glm::vec3 p22 = glm::vec3(
                    radius * sin(phi2) * cos(theta2),
                    radius * cos(phi2),
                    radius * sin(phi2) * sin(theta2)
                );
                
                glm::vec2 uv11 = glm::vec2(static_cast<float>(seg) / segments, static_cast<float>(ring) / rings);
                glm::vec2 uv12 = glm::vec2(static_cast<float>(seg + 1) / segments, static_cast<float>(ring) / rings);
                glm::vec2 uv21 = glm::vec2(static_cast<float>(seg) / segments, static_cast<float>(ring + 1) / rings);
                glm::vec2 uv22 = glm::vec2(static_cast<float>(seg + 1) / segments, static_cast<float>(ring + 1) / rings);
                
                glm::vec3 n11 = glm::normalize(p11);
                glm::vec3 n12 = glm::normalize(p12);
                glm::vec3 n21 = glm::normalize(p21);
                glm::vec3 n22 = glm::normalize(p22);
                
                vertices.push_back(Vertex(p11, uv11, n11));
                vertices.push_back(Vertex(p21, uv21, n21));
                vertices.push_back(Vertex(p12, uv12, n12));
                
                vertices.push_back(Vertex(p12, uv12, n12));
                vertices.push_back(Vertex(p21, uv21, n21));
                vertices.push_back(Vertex(p22, uv22, n22));
            }
        }
        
        return vertices;
    }

    ObjItem Geometries::GenerateCube(Window& window, float size, bool& firstTimeRef, int materialId, bool debug) {
        auto vertices = GenerateCube(size);
        auto mesh = std::make_unique<Mesh>(vertices, window, "cube", firstTimeRef, materialId, debug);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));
        return std::move(ObjItem(std::move(meshes), std::vector<Material>{}));
    }

    ObjItem Geometries::GeneratePlane(Window& window, float width, float height, bool& firstTimeRef, int materialId, bool debug) {
        auto vertices = GeneratePlane(width, height);
        auto mesh = std::make_unique<Mesh>(vertices, window, "plane", firstTimeRef, materialId, debug);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));
        return ObjItem(std::move(meshes), std::vector<Material>{});
    }

    ObjItem Geometries::GenerateSphere(Window& window, float radius, int segments, int rings, bool& firstTimeRef, int materialId, bool debug) {
        auto vertices = GenerateSphere(radius, segments, rings);
        auto mesh = std::make_unique<Mesh>(vertices, window, "sphere", firstTimeRef, materialId, debug);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));
        return ObjItem(std::move(meshes), std::vector<Material>{});
    }

}
