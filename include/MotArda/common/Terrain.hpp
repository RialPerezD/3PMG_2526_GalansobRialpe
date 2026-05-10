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
        Terrain(float width, float depth, float maxHeight, int seed)
			: width(width), depth(depth), maxHeight(maxHeight), seed_(seed) {
		}

        std::shared_ptr<ObjItem> ObjItem_;

        float GetHeightAt(
            float worldX,
            float worldZ
        );

    private:
		friend class MotardaEng;

        float width;
        float depth;
        float maxHeight;
        int seed_;

        void CalculateNormals(
            std::vector<Vertex>& vertices,
            const std::vector<uint32_t>& indices
        );

        void GenerateProcedural(
            int resolution,
            Window& window,
            bool& firstTime,
            int textureId = 0,
            bool debug = false
        );
    };

}