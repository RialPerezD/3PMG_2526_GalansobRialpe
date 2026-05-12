#include <MotArda/Components/RenderComponent.hpp>

namespace MTRD {
    RenderComponent::RenderComponent() :
        objitem_(nullptr) {}

    RenderComponent::RenderComponent(std::shared_ptr<ObjItem> obj) {
        objitem_ = obj;
    }
}
