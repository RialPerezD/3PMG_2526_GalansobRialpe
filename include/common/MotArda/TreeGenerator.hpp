#pragma once

#include <MotArda/Vertex.hpp>
#include <MotArda/ObjItem.hpp>
#include <MotArda/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>
#include <memory>


namespace MTRD {

    class TreeGenerator {
    public:
        struct LSystemRule {
            char symbol;
            std::string replacement;
        };

        struct TreeProfile {
            std::string axiom = "X";
            std::vector<LSystemRule> rules;
            int iterations = 4;
            float baseAngle = 25.0f;
            float angleVariance = 5.0f;
            float baseLength = 1.0f;
            float lengthVariance = 0.2f;
            float widthStep = 0.8f;
            float initialWidth = 0.2f;
            float leafChance = 0.8f;
            float leafSize = 0.5f;
            int radialSegments = 8;
        };

        static ObjItem GenerateTree(Window& window, const TreeProfile& profile, bool& firstTime, unsigned int seed = 12345, int textureId = 0, bool debug = false);

    private:
        struct TransformState {
            glm::vec3 position;
            glm::quat rotation;
            float width;
            int lastRingStartIndex;
            float accumulatedLength;
        };

        struct TrunkResult {
            std::vector<Vertex> vertices;
            std::vector<int> triangles;
        };

        static std::string ExpandLSystem(const TreeProfile& profile);
        static TrunkResult GenerateTrunkMesh(const TreeProfile& profile, std::vector<glm::vec3>& leafPositions, std::vector<glm::quat>& leafRotations, unsigned int& seed);
        static int CreateRing(std::vector<Vertex>& vertices, const glm::vec3& center, const glm::quat& rotation, float radius, float vCoord, int radialSegments);
        static void StitchRings(std::vector<int>& triangles, int ringA, int ringB, int radialSegments);
        static void ComputeNormals(std::vector<Vertex>& vertices, const std::vector<int>& triangles);
        static std::vector<Vertex> GenerateLeafCube(float size);
        static float RandomFloat(unsigned int& seed);
    };

}
