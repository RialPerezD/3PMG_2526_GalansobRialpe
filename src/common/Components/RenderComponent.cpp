#include "MotArda/common/Components/RenderComponent.hpp"

namespace MTRD {
    RenderComponent::RenderComponent() :
        meshes_(new std::vector<Mesh>()),
        materials_(new std::vector<Material>()) {}

    RenderComponent::RenderComponent(
        std::vector<Mesh>* meshes,
        std::vector<Material>* mats){
        meshes_ = meshes;
        materials_ = mats;
    }
}
