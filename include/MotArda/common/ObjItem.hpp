#pragma once
#include <vector>
#include <memory>
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

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
