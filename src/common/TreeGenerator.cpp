#include <MotArda/TreeGenerator.hpp>
#include <MotArda/Mesh.hpp>
#include <MotArda/Material.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <stack>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>


namespace MTRD {

    float TreeGenerator::RandomFloat(unsigned int& seed) {
        seed = seed * 1103515245 + 12345;
        return (float)((seed >> 16) & 0x7FFF) / 32768.0f;
    }

    std::string TreeGenerator::ExpandLSystem(const TreeProfile& profile) {
        std::string current = profile.axiom;
        for (int i = 0; i < profile.iterations; i++) {
            std::string result;
            for (char c : current) {
                bool replaced = false;
                for (const auto& rule : profile.rules) {
                    if (rule.symbol == c) {
                        result += rule.replacement;
                        replaced = true;
                        break;
                    }
                }
                if (!replaced) {
                    result += c;
                }
            }
            current = result;
        }
        return current;
    }

    int TreeGenerator::CreateRing(std::vector<Vertex>& vertices, const glm::vec3& center, const glm::quat& rotation, float radius, float vCoord, int radialSegments) {
        int startIndex = (int)vertices.size();
        for (int i = 0; i <= radialSegments; i++) {
            float t = (float)i / (float)radialSegments;
            float ang = t * glm::pi<float>() * 2.0f;
            glm::vec3 ringPos = glm::vec3(cos(ang) * radius, 0.0f, sin(ang) * radius);
            ringPos = center + (rotation * ringPos);
            glm::vec2 uv = glm::vec2(t, vCoord);
            vertices.push_back(Vertex(ringPos, uv, glm::vec3(0.0f)));
        }
        return startIndex;
    }

    void TreeGenerator::StitchRings(std::vector<int>& triangles, int ringA, int ringB, int radialSegments) {
        for (int i = 0; i < radialSegments; i++) {
            int a = ringA + i;
            int b = ringB + i;
            int nextA = ringA + i + 1;
            int nextB = ringB + i + 1;
            triangles.push_back(a);
            triangles.push_back(b);
            triangles.push_back(nextB);
            triangles.push_back(a);
            triangles.push_back(nextB);
            triangles.push_back(nextA);
        }
    }

    void TreeGenerator::ComputeNormals(std::vector<Vertex>& vertices, const std::vector<int>& triangles) {
        for (auto& v : vertices) {
            v.normal = glm::vec3(0.0f);
        }

        for (size_t i = 0; i < triangles.size(); i += 3) {
            int i0 = triangles[i];
            int i1 = triangles[i + 1];
            int i2 = triangles[i + 2];
            glm::vec3 v0 = vertices[i0].position;
            glm::vec3 v1 = vertices[i1].position;
            glm::vec3 v2 = vertices[i2].position;
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            vertices[i0].normal += normal;
            vertices[i1].normal += normal;
            vertices[i2].normal += normal;
        }

        for (auto& v : vertices) {
            float len = glm::length(v.normal);
            if (len > 0.0001f) {
                v.normal /= len;
            }
        }
    }

    TreeGenerator::TrunkResult TreeGenerator::GenerateTrunkMesh(
        const TreeProfile& profile,
        std::vector<glm::vec3>& leafPositions,
        std::vector<glm::quat>& leafRotations,
        unsigned int& seed)
    {
        TrunkResult result;
        std::string lstring = ExpandLSystem(profile);

        glm::vec3 pos(0.0f);
        glm::quat rot(1.0f, 0.0f, 0.0f, 0.0f);
        float currentWidth = profile.initialWidth;
        float accumulatedLength = 0.0f;

        int lastRingStartIndex = CreateRing(result.vertices, pos, rot, currentWidth, 0.0f, profile.radialSegments);

        std::stack<TransformState> stateStack;

        for (char c : lstring) {
            switch (c) {
                case 'F': {
                    float lengthOffset = (RandomFloat(seed) * 2.0f - 1.0f) * profile.lengthVariance;
                    float randomLength = profile.baseLength + lengthOffset;
                    pos += rot * glm::vec3(0.0f, 1.0f, 0.0f) * randomLength;
                    accumulatedLength += randomLength;
                    currentWidth *= profile.widthStep;
                    int ringStart = CreateRing(result.vertices, pos, rot, currentWidth, accumulatedLength, profile.radialSegments);
                    StitchRings(result.triangles, lastRingStartIndex, ringStart, profile.radialSegments);
                    lastRingStartIndex = ringStart;
                    break;
                }
                case 'L': {
                    if (RandomFloat(seed) < profile.leafChance) {
                        leafPositions.push_back(pos);
                        leafRotations.push_back(rot);
                    }
                    break;
                }
                case '+': {
                    float angle = profile.baseAngle + (RandomFloat(seed) * 2.0f - 1.0f) * profile.angleVariance;
                    glm::quat tilt = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
                    glm::quat yaw = glm::angleAxis(glm::radians(RandomFloat(seed) * 360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    rot *= (yaw * tilt);
                    break;
                }
                case '-': {
                    float angle = profile.baseAngle + (RandomFloat(seed) * 2.0f - 1.0f) * profile.angleVariance;
                    glm::quat tilt = glm::angleAxis(glm::radians(-angle), glm::vec3(0.0f, 0.0f, 1.0f));
                    glm::quat yaw = glm::angleAxis(glm::radians(RandomFloat(seed) * 360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    rot *= (yaw * tilt);
                    break;
                }
                case '[': {
                    TransformState state;
                    state.position = pos;
                    state.rotation = rot;
                    state.width = currentWidth;
                    state.lastRingStartIndex = lastRingStartIndex;
                    state.accumulatedLength = accumulatedLength;
                    stateStack.push(state);
                    break;
                }
                case ']': {
                    if (!stateStack.empty()) {
                        TransformState state = stateStack.top();
                        stateStack.pop();
                        pos = state.position;
                        rot = state.rotation;
                        currentWidth = state.width;
                        lastRingStartIndex = state.lastRingStartIndex;
                        accumulatedLength = state.accumulatedLength;
                    }
                    break;
                }
            }
        }

        ComputeNormals(result.vertices, result.triangles);

        return result;
    }

    std::vector<Vertex> TreeGenerator::GenerateLeafCube(float size) {
        float h = size * 0.5f;

        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-h, -h, h), glm::vec2(0, 1), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(1, 1), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(h, h, h), glm::vec2(1, 0), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(-h, -h, h), glm::vec2(0, 1), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(h, h, h), glm::vec2(1, 0), glm::vec3(0, 0, 1)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(0, 0), glm::vec3(0, 0, 1)),

            Vertex(glm::vec3(h, -h, -h), glm::vec2(0, 1), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(1, 1), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(1, 0), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(h, -h, -h), glm::vec2(0, 1), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(1, 0), glm::vec3(0, 0, -1)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(0, 0), glm::vec3(0, 0, -1)),

            Vertex(glm::vec3(-h, h, h), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(h, h, h), glm::vec2(1, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(0, 0), glm::vec3(0, 1, 0)),

            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(h, -h, -h), glm::vec2(1, 1), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
            Vertex(glm::vec3(-h, -h, h), glm::vec2(0, 0), glm::vec3(0, -1, 0)),

            Vertex(glm::vec3(h, -h, h), glm::vec2(0, 1), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, -h, -h), glm::vec2(1, 1), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, -h, h), glm::vec2(0, 1), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, h, -h), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
            Vertex(glm::vec3(h, h, h), glm::vec2(0, 0), glm::vec3(1, 0, 0)),

            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, -h, h), glm::vec2(1, 1), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(1, 0), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, -h, -h), glm::vec2(0, 1), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, h, h), glm::vec2(1, 0), glm::vec3(-1, 0, 0)),
            Vertex(glm::vec3(-h, h, -h), glm::vec2(0, 0), glm::vec3(-1, 0, 0)),
        };

        return vertices;
    }

    ObjItem TreeGenerator::GenerateTree(Window& window, const TreeProfile& profile, bool& firstTime, unsigned int seed, int textureId, bool debug) {
        std::vector<glm::vec3> leafPositions;
        std::vector<glm::quat> leafRotations;

        TrunkResult trunk = GenerateTrunkMesh(profile, leafPositions, leafRotations, seed);

        auto trunkVertices = trunk.vertices;
        auto trunkTriangles = trunk.triangles;

        // Convert indexed trunk mesh to non-indexed vertex list
        std::vector<Vertex> trunkFinalVerts;
        trunkFinalVerts.reserve(trunkTriangles.size());
        for (int idx : trunkTriangles) {
            trunkFinalVerts.push_back(trunkVertices[idx]);
        }

        // Build leaf mesh
        std::vector<Vertex> leafVerts;
        std::vector<Vertex> baseCube = GenerateLeafCube(profile.leafSize);

        for (size_t i = 0; i < leafPositions.size(); i++) {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), leafPositions[i])
                                * glm::mat4_cast(leafRotations[i]);

            glm::mat3 normalMat = glm::mat3(leafRotations[i]);

            for (const auto& v : baseCube) {
                glm::vec4 newPos = transform * glm::vec4(v.position, 1.0f);
                glm::vec3 newNorm = glm::normalize(normalMat * v.normal);
                leafVerts.push_back(Vertex(glm::vec3(newPos), v.uv, newNorm));
            }
        }

        std::vector<std::unique_ptr<Mesh>> meshes;

        auto trunkMesh = std::make_unique<Mesh>(trunkFinalVerts, window, "tree_trunk", firstTime, 0, debug);
        meshes.push_back(std::move(trunkMesh));

        std::string leafName = "tree_leaves_" + std::to_string(leafPositions.size());
        auto leavesMesh = std::make_unique<Mesh>(leafVerts, window, leafName, firstTime, 1, debug);
        meshes.push_back(std::move(leavesMesh));

        std::vector<Material> materials;

        Material trunkMaterial;
        trunkMaterial.name = "trunk";
        trunkMaterial.diffuse = glm::vec3(0.545f, 0.271f, 0.075f);
#ifndef __SWITCH__
        {
            unsigned char brown[3] = { 139, 69, 19 };
            GLuint texId;
            glCreateTextures(GL_TEXTURE_2D, 1, &texId);
            glTextureStorage2D(texId, 1, GL_RGB8, 1, 1);
            glTextureSubImage2D(texId, 0, 0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, brown);
            glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            trunkMaterial.diffuseTexID = texId;
        }
#endif
        materials.push_back(trunkMaterial);

        Material leafMaterial;
        leafMaterial.name = "leaf";
        leafMaterial.diffuse = glm::vec3(0.0f, 0.6f, 0.0f);
#ifndef __SWITCH__
        {
            unsigned char green[3] = { 0, 128, 0 };
            GLuint texId;
            glCreateTextures(GL_TEXTURE_2D, 1, &texId);
            glTextureStorage2D(texId, 1, GL_RGB8, 1, 1);
            glTextureSubImage2D(texId, 0, 0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, green);
            glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            leafMaterial.diffuseTexID = texId;
        }
#endif
        materials.push_back(leafMaterial);

        return ObjItem(std::move(meshes), materials);
    }

}
