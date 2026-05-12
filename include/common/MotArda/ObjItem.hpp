#pragma once

#include <MotArda/Mesh.hpp>
#include <MotArda/Material.hpp>

#include <vector>
#include <memory>

namespace MTRD {
    struct ObjItem {
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<Material> materials;

        ObjItem() = default;
        ~ObjItem();
        ObjItem(ObjItem&& other);
        ObjItem& operator=(ObjItem&& other);
        ObjItem(const ObjItem&) = delete;
        ObjItem& operator=(const ObjItem&) = delete;
        ObjItem(std::vector<std::unique_ptr<Mesh>> m, std::vector<Material> mat);
    };
}
