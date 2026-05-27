//Desactivar macros de windows pra poder usar min y max de glm
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define FNL_IMPL
#include <FastNoiseLite.h>

#include <MotArda/Terrain.hpp>

#include <glad/glad.h>
#include <random>
#include <cmath>
#include <vector>
#include <sstream>

namespace MTRD {

    static fnl_state InitNoise(int seed) {
        fnl_state noise = fnlCreateState();
        noise.seed = seed;
        noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
        noise.fractal_type = FNL_FRACTAL_FBM;
        noise.octaves = 5;
        noise.lacunarity = 2.0f;
        noise.gain = 0.5f;
        noise.frequency = 0.01f;
        return noise;
    }

    static float SampleHeight(fnl_state& noise, float maxHeight, float worldX, float worldZ) {
        float rawNoise = fnlGetNoise2D(&noise, worldX, worldZ);
        float normalizedNoise = (rawNoise + 1.0f) * 0.5f;
        float height = normalizedNoise * normalizedNoise * maxHeight;
        height += pow(normalizedNoise, 4.0f) * (maxHeight * 0.35f);
        return height;
    }

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
        fnl_state noise = InitNoise(seed_);

        const float lakeLevel = maxHeight * 0.20f;
        const float grassLevel = maxHeight * 0.35f;
        const float forestLevel = maxHeight * 0.50f;
        const float rockLevel = maxHeight * 0.70f;

        float halfWidth = width * 0.5f;
        float halfDepth = depth * 0.5f;

        const int CHUNK_SIZE = 16;
        const int NUM_CHUNKS_X = resolution / CHUNK_SIZE;
        const int NUM_CHUNKS_Z = resolution / CHUNK_SIZE;
        const float CHUNK_WORLD_SIZE_X = width / NUM_CHUNKS_X;
        const float CHUNK_WORLD_SIZE_Z = depth / NUM_CHUNKS_Z;
        const int LOD_STEPS[4] = { 1, 2, 4, 8 };

        std::vector<std::unique_ptr<Mesh>> meshes;

        for (int cz = 0; cz < NUM_CHUNKS_Z; cz++) {
            for (int cx = 0; cx < NUM_CHUNKS_X; cx++) {
                for (int lodIdx = 0; lodIdx < 4; lodIdx++) {
                    int step = LOD_STEPS[lodIdx];
                    int vertsPerEdge = CHUNK_SIZE / step;
                    int numVerts = (vertsPerEdge + 1) * (vertsPerEdge + 1);

                    std::vector<Vertex> vertices;
                    vertices.reserve(numVerts);

                    int zStart = cz * CHUNK_SIZE;
                    int zEnd = (cz + 1) * CHUNK_SIZE;
                    int xStart = cx * CHUNK_SIZE;
                    int xEnd = (cx + 1) * CHUNK_SIZE;

                    for (int z = zStart; z <= zEnd; z += step) {
                        for (int x = xStart; x <= xEnd; x += step) {
                            float percentX = static_cast<float>(x) / resolution;
                            float percentZ = static_cast<float>(z) / resolution;
                            float worldX = percentX * width - halfWidth;
                            float worldZ = percentZ * depth - halfDepth;
                            float height = SampleHeight(noise, maxHeight, worldX, worldZ);

                            Vertex vertex{};
                            vertex.position = glm::vec3(worldX, height, worldZ);
                            vertex.uv = glm::vec2(percentX, percentZ);
                            vertices.push_back(vertex);
                        }
                    }

                    std::vector<Vertex> triangleList;
                    triangleList.reserve(vertsPerEdge * vertsPerEdge * 6);

                    for (int z = 0; z < vertsPerEdge; z++) {
                        for (int x = 0; x < vertsPerEdge; x++) {
                            int i0 = x + z * (vertsPerEdge + 1);
                            int i1 = (x + 1) + z * (vertsPerEdge + 1);
                            int i2 = x + (z + 1) * (vertsPerEdge + 1);
                            int i3 = (x + 1) + (z + 1) * (vertsPerEdge + 1);

                            triangleList.push_back(vertices[i0]);
                            triangleList.push_back(vertices[i2]);
                            triangleList.push_back(vertices[i1]);

                            triangleList.push_back(vertices[i1]);
                            triangleList.push_back(vertices[i2]);
                            triangleList.push_back(vertices[i3]);
                        }
                    }

                    std::vector<uint32_t> dummyIndices(triangleList.size());
                    for (size_t i = 0; i < dummyIndices.size(); ++i)
                        dummyIndices[i] = static_cast<uint32_t>(i);

                    CalculateNormals(triangleList, dummyIndices);

                    glm::vec3 minPos = triangleList[0].position;
                    glm::vec3 maxPos = triangleList[0].position;
                    for (const auto& v : triangleList) {
                        minPos = glm::min(minPos, v.position);
                        maxPos = glm::max(maxPos, v.position);
                    }

                    float chunkCenterX = (cx + 0.5f) * CHUNK_WORLD_SIZE_X - halfWidth;
                    float chunkCenterZ = (cz + 0.5f) * CHUNK_WORLD_SIZE_Z - halfDepth;

                    std::ostringstream nameStream;
                    nameStream << "terrain_" << cx << "_" << cz
                        << "_nc" << NUM_CHUNKS_X
                        << "_lod" << lodIdx
                        << "_wx" << chunkCenterX
                        << "_wz" << chunkCenterZ;
                    std::string meshName = nameStream.str();

                    auto mesh = std::make_unique<Mesh>(
                        triangleList, window, meshName,
                        firstTime, textureId, debug
                    );
                    mesh->aabbMin = minPos;
                    mesh->aabbMax = maxPos;
                    meshes.push_back(std::move(mesh));
                }
            }
        }

        const float lutLevels[5] = {
            0.0f,
            lakeLevel / maxHeight,
            grassLevel / maxHeight,
            forestLevel / maxHeight,
            rockLevel / maxHeight
        };

        const glm::vec3 lutColors[5] = {
            glm::vec3(0.35f, 0.22f, 0.12f),
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

        lutTexture;
        glCreateTextures(GL_TEXTURE_2D, 1, &lutTexture.id_);
        glTextureStorage2D(lutTexture.id_, 1, GL_RGB8, LUT_SIZE, 1);
        glTextureSubImage2D(lutTexture.id_, 0, 0, 0, LUT_SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, lutData.data());
        glTextureParameteri(lutTexture.id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(lutTexture.id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(lutTexture.id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(lutTexture.id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Material terrainMat;
        terrainMat.name = "terrain_diffuse";
        terrainMat.diffuseTexID = lutTexture.getId();
        terrainMat.useHeightLUT = true;
        terrainMat.maxHeight = maxHeight;
        terrainMat.specular = glm::vec3(0.0f);
        terrainMat.shininess = 1.0f;

        std::vector<Material> materials;
        materials.push_back(terrainMat);

        ObjItem_ = std::make_shared<ObjItem>(std::move(meshes), materials);
    }

    float Terrain::GetHeightAt(float worldX, float worldZ) {
        fnl_state noise = InitNoise(seed_);

        float rawNoise = fnlGetNoise2D(&noise, worldX, worldZ);
        float normalizedNoise = (rawNoise + 1.0f) * 0.5f;

        float continentalness = normalizedNoise * normalizedNoise;
        float mountainMask = pow(normalizedNoise, 4.0f);

        float height = continentalness * maxHeight;
        height += mountainMask * (maxHeight * 0.35f);

        return height;
    }
}