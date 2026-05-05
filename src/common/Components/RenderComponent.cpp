#include "MotArda/common/Components/RenderComponent.hpp"

namespace MTRD {
    RenderComponent::RenderComponent() :
        meshes_(nullptr),
        materials_(nullptr) {}

    RenderComponent::RenderComponent(
        std::vector<std::unique_ptr<Mesh>>* meshes,
        std::vector<Material>* mats){
        meshes_ = meshes;
        materials_ = mats;
    }
}
