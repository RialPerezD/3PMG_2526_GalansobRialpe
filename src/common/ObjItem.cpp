#include "MotArda/common/ObjItem.hpp"

namespace MTRD {
    ObjItem::~ObjItem() = default;

    ObjItem::ObjItem(ObjItem&& other)
        : meshes(std::move(other.meshes)), materials(std::move(other.materials)) {}

    ObjItem& ObjItem::operator=(ObjItem&& other) {
        if (this != &other) {
            meshes = std::move(other.meshes);
            materials = std::move(other.materials);
        }
        return *this;
    }

    ObjItem::ObjItem(std::vector<std::unique_ptr<Mesh>> m, std::vector<Material> mat)
        : meshes(std::move(m)), materials(std::move(mat)) {}
}
