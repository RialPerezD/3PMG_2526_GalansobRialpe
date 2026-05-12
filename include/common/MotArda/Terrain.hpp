#pragma once

#include <MotArda/Vertex.hpp>
#include <MotArda/window.hpp>
#include <MotArda/Mesh.hpp>
#include <MotArda/Material.hpp>

#include <vector>
#include <string>


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