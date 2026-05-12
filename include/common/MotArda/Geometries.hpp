#pragma once

#include <MotArda/Vertex.hpp>
#include <MotArda/window.hpp>
#include <MotArda/Mesh.hpp>
#include <MotArda/Material.hpp>

#include <vector>
#include <memory>


namespace MTRD {
    class Geometries {
    public:
        static std::vector<Vertex> GenerateCube(float size = 1.0f);
        static std::vector<Vertex> GeneratePlane(float width = 1.0f, float height = 1.0f);
        static std::vector<Vertex> GenerateSphere(float radius = 0.5f, int segments = 32, int rings = 16);

        static ObjItem GenerateCube(Window& window, float size, bool& firstTime, int texureId = 0, bool debug = false);
        static ObjItem GeneratePlane(Window& window, float width, float height, bool& firstTime, int texureId = 0, bool debug = false);
        static ObjItem GenerateSphere(Window& window, float radius, int segments, int rings, bool& firstTime, int texureId = 0, bool debug = false);
    };
}
