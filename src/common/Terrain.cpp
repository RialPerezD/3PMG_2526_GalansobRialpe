#define FNL_IMPL
#include "FastNoiseLite.h"

#include "MotArda/common/Terrain.hpp"

#include <glad/glad.h>
#include <random>
#include <cmath>
#include <vector>

namespace MTRD {

    void Terrain::CalculateNormals(
        std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices
    ) {
        for (auto& v : vertices)
            v.normal = glm::vec3(0.0f);

        for (size_t i = 0; i < indices.size(); i += 3) {
            Vertex& v0 = vertices[indices[i]];
            Vertex& v1 = vertices[indices[i + 1]];
            Vertex& v2 = vertices[indices[i + 2]];

            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;

            glm::vec3 normal = glm::cross(edge1, edge2);

            v0.normal += normal;
            v1.normal += normal;
            v2.normal += normal;
        }

        for (auto& v : vertices) {
            if (glm::length(v.normal) > 0.0f)
                v.normal = glm::normalize(v.normal);
            else
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }

    void Terrain::GenerateProcedural(
        int resolution,
        Window& window,
        bool& firstTime,
        int textureId,
        bool debug
    ) {
        std::vector<Vertex> gridVertices;
        gridVertices.reserve((resolution + 1) * (resolution + 1));

        fnl_state noise = fnlCreateState();
        noise.seed = seed_;
        noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
        noise.fractal_type = FNL_FRACTAL_FBM;
        noise.octaves = 5;
        noise.lacunarity = 2.0f;
        noise.gain = 0.5f;
        noise.frequency = 0.01f;

        const float lakeLevel = maxHeight * 0.20f;
        const float grassLevel = maxHeight * 0.35f;
        const float forestLevel = maxHeight * 0.50f;
        const float rockLevel = maxHeight * 0.70f;
        const float snowLevel = maxHeight * 0.85f;

        float halfWidth = width * 0.5f;
        float halfDepth = depth * 0.5f;

        for (int z = 0; z <= resolution; z++) {
            for (int x = 0; x <= resolution; x++) {
                float percentX = static_cast<float>(x) / resolution;
                float percentZ = static_cast<float>(z) / resolution;

                float worldX = percentX * width - halfWidth;
                float worldZ = percentZ * depth - halfDepth;

                float rawNoise = fnlGetNoise2D(&noise, worldX, worldZ);
                float normalizedNoise = (rawNoise + 1.0f) * 0.5f;

                float continentalness = normalizedNoise * normalizedNoise;
                float mountainMask = pow(normalizedNoise, 4.0f);

                float height = continentalness * maxHeight;
                height += mountainMask * (maxHeight * 0.35f);

                Vertex vertex{};
                vertex.position = glm::vec3(worldX, height, worldZ);
                vertex.uv = glm::vec2(percentX, percentZ);
                gridVertices.push_back(vertex);
            }
        }

        std::vector<Vertex> triangleList;
        triangleList.reserve(resolution * resolution * 6);

        auto getIdx = [resolution](int x, int z) {
            return x + (z * (resolution + 1));
            };

        for (int z = 0; z < resolution; z++) {
            for (int x = 0; x < resolution; x++) {
                int i0 = getIdx(x, z);
                int i1 = getIdx(x + 1, z);
                int i2 = getIdx(x, z + 1);
                int i3 = getIdx(x + 1, z + 1);

                triangleList.push_back(gridVertices[i0]);
                triangleList.push_back(gridVertices[i2]);
                triangleList.push_back(gridVertices[i1]);

                triangleList.push_back(gridVertices[i1]);
                triangleList.push_back(gridVertices[i2]);
                triangleList.push_back(gridVertices[i3]);
            }
        }

        std::vector<uint32_t> dummyIndices(triangleList.size());
        for (size_t i = 0; i < dummyIndices.size(); ++i) dummyIndices[i] = static_cast<uint32_t>(i);

        CalculateNormals(triangleList, dummyIndices);

        auto mesh = std::make_unique<Mesh>(
            triangleList,
            window,
            "terrain",
            firstTime,
            textureId,
            debug
        );

        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));

        const float lutLevels[5] = {
            0.0f,
            lakeLevel / maxHeight,
            grassLevel / maxHeight,
            forestLevel / maxHeight,
            rockLevel / maxHeight
        };

        const glm::vec3 lutColors[5] = {
            glm::vec3(0.10f, 0.30f, 0.90f),
            glm::vec3(0.30f, 0.65f, 0.20f),
            glm::vec3(0.15f, 0.40f, 0.10f),
            glm::vec3(0.60f, 0.60f, 0.58f),
            glm::vec3(0.92f, 0.92f, 0.95f)
        };

        const int LUT_SIZE = 256;
        std::vector<unsigned char> lutData(LUT_SIZE * 3);

        for (int i = 0; i < LUT_SIZE; i++) {
            float t = i / (float)(LUT_SIZE - 1);
            int seg = 0;
            for (int j = 0; j < 4; j++) {
                if (t >= lutLevels[j]) seg = j;
            }

            float segStart = lutLevels[seg];
            float segEnd = (seg < 4) ? lutLevels[seg + 1] : 1.0f;
            float segRange = segEnd - segStart;
            float localT = (segRange > 0.0f) ? glm::clamp((t - segStart) / segRange, 0.0f, 1.0f) : 0.0f;

            glm::vec3 color = glm::mix(lutColors[seg], lutColors[seg + 1], localT);

            lutData[i * 3 + 0] = (unsigned char)(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
            lutData[i * 3 + 1] = (unsigned char)(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
            lutData[i * 3 + 2] = (unsigned char)(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
        }

        GLuint lutTexture;
        glCreateTextures(GL_TEXTURE_2D, 1, &lutTexture);
        glTextureStorage2D(lutTexture, 1, GL_RGB8, LUT_SIZE, 1);
        glTextureSubImage2D(lutTexture, 0, 0, 0, LUT_SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, lutData.data());
        glTextureParameteri(lutTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(lutTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(lutTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(lutTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Material terrainMat;
        terrainMat.name = "terrain_diffuse";
        terrainMat.diffuseTexID = lutTexture;
        terrainMat.useHeightLUT = true;
        terrainMat.maxHeight = maxHeight;
        terrainMat.specular = glm::vec3(0.0f);
        terrainMat.shininess = 1.0f;

        std::vector<Material> materials;
        materials.push_back(terrainMat);

        ObjItem_ = std::make_shared<ObjItem>(std::move(meshes), materials);
    }

    float Terrain::GetHeightAt(float worldX, float worldZ) {
        fnl_state noise = fnlCreateState();
        noise.seed = seed_;
        noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
        noise.fractal_type = FNL_FRACTAL_FBM;
        noise.octaves = 5;
        noise.lacunarity = 2.0f;
        noise.gain = 0.5f;
        noise.frequency = 0.01f;

        float rawNoise = fnlGetNoise2D(&noise, worldX, worldZ);
        float normalizedNoise = (rawNoise + 1.0f) * 0.5f;

        float continentalness = normalizedNoise * normalizedNoise;
        float mountainMask = pow(normalizedNoise, 4.0f);

        float height = continentalness * maxHeight;
        height += mountainMask * (maxHeight * 0.35f);

        return height;
    }
}