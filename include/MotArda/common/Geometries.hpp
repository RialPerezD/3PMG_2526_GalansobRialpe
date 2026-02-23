#pragma once
#include <vector>
#include <memory>
#include <MotArda/common/Vertex.hpp>
#include <MotArda/win64/window.hpp>
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

namespace MTRD {
    class Geometries {
    public:
        static std::vector<Vertex> GenerateCube(float size = 1.0f);
        static std::vector<Vertex> GeneratePlane(float width = 1.0f, float height = 1.0f);
        static std::vector<Vertex> GenerateSphere(float radius = 0.5f, int segments = 32, int rings = 16);

        static ObjItem GenerateCube(Window& window, float size, bool& firstTime, bool debug = false);
        static ObjItem GeneratePlane(Window& window, float width, float height, bool& firstTime, bool debug = false);
        static ObjItem GenerateSphere(Window& window, float radius, int segments, int rings, bool& firstTime, bool debug = false);
    };
}
