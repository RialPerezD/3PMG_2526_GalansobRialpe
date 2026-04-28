#include "MotArda/common/Terrain.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include "stb_image.h"

namespace MTRD {
    std::string Terrain::GetDefaultHeightmapPath(){
        return std::string();
    }


    ObjItem Terrain::GenerateFromHeightmap(const std::string& heightmapPath, float width, float depth, float maxHeight, Window& window, bool& firstTime, int textureId, bool debug) {
        int texWidth, texHeight, channels;
        unsigned char* data = stbi_load(heightmapPath.c_str(), &texWidth, &texHeight, &channels, 1);

        std::vector<Vertex> vertices;

        if (!data) {
            return ObjItem({}, {});
        }

        auto getHeight = [&](int x, int z) {
            x = std::clamp(x, 0, static_cast<int>(texWidth) - 1);
            z = std::clamp(z, 0, static_cast<int>(texHeight) - 1);
            return (data[z * texWidth + x] / 255.0f) * maxHeight;
            };

        auto getNormal = [&](int x, int z) {
            float hL = getHeight(x - 1, z);
            float hR = getHeight(x + 1, z);
            float hD = getHeight(x, z - 1);
            float hU = getHeight(x, z + 1);
            return glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
            };

        for (int z = 0; z < texHeight - 1; ++z) {
            for (int x = 0; x < texWidth - 1; ++x) {
                float x0 = (static_cast<float>(x) / (texWidth - 1) - 0.5f) * width;
                float x1 = (static_cast<float>(x + 1) / (texWidth - 1) - 0.5f) * width;
                float z0 = (static_cast<float>(z) / (texHeight - 1) - 0.5f) * depth;
                float z1 = (static_cast<float>(z + 1) / (texHeight - 1) - 0.5f) * depth;

                glm::vec3 p00(x0, getHeight(x, z), z0);
                glm::vec3 p10(x1, getHeight(x + 1, z), z0);
                glm::vec3 p01(x0, getHeight(x, z + 1), z1);
                glm::vec3 p11(x1, getHeight(x + 1, z + 1), z1);

                glm::vec2 uv00(static_cast<float>(x) / (texWidth - 1), static_cast<float>(z) / (texHeight - 1));
                glm::vec2 uv10(static_cast<float>(x + 1) / (texWidth - 1), static_cast<float>(z) / (texHeight - 1));
                glm::vec2 uv01(static_cast<float>(x) / (texWidth - 1), static_cast<float>(z + 1) / (texHeight - 1));
                glm::vec2 uv11(static_cast<float>(x + 1) / (texWidth - 1), static_cast<float>(z + 1) / (texHeight - 1));

                vertices.push_back(Vertex(p00, uv00, getNormal(x, z)));
                vertices.push_back(Vertex(p01, uv01, getNormal(x, z + 1)));
                vertices.push_back(Vertex(p10, uv10, getNormal(x + 1, z)));

                vertices.push_back(Vertex(p10, uv10, getNormal(x + 1, z)));
                vertices.push_back(Vertex(p01, uv01, getNormal(x, z + 1)));
                vertices.push_back(Vertex(p11, uv11, getNormal(x + 1, z + 1)));
            }
        }

        stbi_image_free(data);

        auto mesh = std::make_unique<Mesh>(vertices, window, "terrain", firstTime, textureId, debug);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));

        std::vector<Material> materials;
        Material terrainMat;
        terrainMat.name = "terrain_diffuse";
        terrainMat.diffuseTexPath = "../assets/textures/terrain/diffuse.jpg";
        materials.push_back(terrainMat);

        return std::move(ObjItem(std::move(meshes), materials));
    }

}