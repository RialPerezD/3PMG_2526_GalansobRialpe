#include "MotArda/common/Components/NetworkComponent.hpp"

MTRD::NetworkComponent::NetworkComponent()
    : networkID(0), meshId_(0), isLocal(true) {
}

MTRD::NetworkComponent::NetworkComponent(uint32_t id, float meshId, bool local)
    : networkID(id), meshId_(meshId), isLocal(local) {
}
