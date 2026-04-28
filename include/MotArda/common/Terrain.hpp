#pragma once
#include <vector>
#include <string>
#include <MotArda/common/Vertex.hpp>
#include <MotArda/win64/window.hpp>
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

namespace MTRD {
    class Terrain {
    public:
        static std::string GetDefaultHeightmapPath();

        static ObjItem GenerateFromHeightmap(
            const std::string& heightmapPath,
            float width,
            float depth,
            float maxHeight,
            Window& window,
            bool& firstTime,
            int textureId = 0,
            bool debug = false);
    };
}