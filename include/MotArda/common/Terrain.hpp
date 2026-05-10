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

        static ObjItem GenerateProcedural(
            int seed,
            int resolution,
            float width,
            float depth,
            float maxHeight,
            Window& window,
            bool& firstTime,
            int textureId = 0,
            bool debug = false
        );

    private:

        static void CalculateNormals(
            std::vector<Vertex>& vertices,
            const std::vector<uint32_t>& indices
        );
    };

}