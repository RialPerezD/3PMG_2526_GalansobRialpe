#define FNL_IMPL
#include "FastNoiseLite.h"

#include "MotArda/common/Terrain.hpp"

#include <random>
#include <cmath>

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

            glm::vec3 edge1 =
                v1.position - v0.position;

            glm::vec3 edge2 =
                v2.position - v0.position;

            glm::vec3 normal =
                glm::cross(edge1, edge2);

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

    ObjItem Terrain::GenerateProcedural(
        int seed,
        int resolution,
        float width,
        float depth,
        float maxHeight,
        Window& window,
        bool& firstTime,
        int textureId,
        bool debug
    ) {

        std::vector<Vertex> gridVertices;
        gridVertices.reserve((resolution + 1) * (resolution + 1));

        // =====================================================
        // FAST NOISE LITE
        // =====================================================

        fnl_state noise = fnlCreateState();

        noise.seed = seed;

        noise.noise_type =
            FNL_NOISE_OPENSIMPLEX2;

        noise.fractal_type =
            FNL_FRACTAL_FBM;

        noise.octaves = 5;

        noise.lacunarity = 2.0f;

        noise.gain = 0.5f;

        noise.frequency = 0.01f;

        // =====================================================
        // BIOME LEVELS
        // =====================================================

        const float lakeLevel =
            maxHeight * 0.15f;

        const float grassLevel =
            maxHeight * 0.30f;

        const float forestLevel =
            maxHeight * 0.50f;

        const float rockLevel =
            maxHeight * 0.70f;

        const float snowLevel =
            maxHeight * 0.85f;

        // =====================================================
        // TERRAIN SIZE
        // =====================================================

        float halfWidth =
            width * 0.5f;

        float halfDepth =
            depth * 0.5f;

        // =====================================================
        // GENERATE VERTICES
        // =====================================================

        for (int z = 0; z <= resolution; z++) {

            for (int x = 0; x <= resolution; x++) {

                float percentX =
                    static_cast<float>(x) /
                    resolution;

                float percentZ =
                    static_cast<float>(z) /
                    resolution;

                float worldX =
                    percentX * width -
                    halfWidth;

                float worldZ =
                    percentZ * depth -
                    halfDepth;

                // =============================================
                // NOISE SAMPLE
                // =============================================

                float rawNoise =
                    fnlGetNoise2D(
                        &noise,
                        worldX,
                        worldZ
                    );

                // [-1,1] -> [0,1]

                float normalizedNoise =
                    (rawNoise + 1.0f) * 0.5f;

                // Terrain shaping

                float continentalness =
                    normalizedNoise *
                    normalizedNoise;

                float mountainMask =
                    pow(normalizedNoise, 4.0f);

                float height =
                    continentalness *
                    maxHeight;

                height +=
                    mountainMask *
                    (maxHeight * 0.35f);

                // =============================================
                // VERTEX
                // =============================================

                Vertex vertex{};

                vertex.position = glm::vec3(
                    worldX,
                    height,
                    worldZ
                );

                vertex.uv = glm::vec2(
                    percentX,
                    percentZ
                );

                gridVertices.push_back(vertex);
            }
        }

        // =====================================================
        // INDICES
        // =====================================================

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

        // =====================================================
        // NORMALS
        // =====================================================

        std::vector<uint32_t> dummyIndices(triangleList.size());
        for (size_t i = 0; i < dummyIndices.size(); ++i) {
            dummyIndices[i] = static_cast<uint32_t>(i);
        }

        CalculateNormals(
            triangleList,
            dummyIndices
        );

        // =====================================================
        // MESH
        // =====================================================

        auto mesh = std::make_unique<Mesh>(
            triangleList,
            window,
            "terrain",
            firstTime,
            textureId,
            debug
        );

        std::vector<std::unique_ptr<Mesh>> meshes;

        meshes.push_back(
            std::move(mesh)
        );

        // =====================================================
        // MATERIAL
        // =====================================================

        std::vector<Material> materials;

        Material terrainMat;

        terrainMat.name =
            "terrain_diffuse";

        terrainMat.diffuseTexPath =
            "../assets/textures/terrain/diffuse.jpg";

        materials.push_back(
            terrainMat
        );

        return ObjItem(
            std::move(meshes),
            materials
        );
    }

}