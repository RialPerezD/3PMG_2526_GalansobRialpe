#include "MotArda/common/Components/NetworkComponent.hpp"

MTRD::NetworkComponent::NetworkComponent()
    : networkID(0), isLocal(true) {
}

MTRD::NetworkComponent::NetworkComponent(uint32_t id, bool local)
    : networkID(id), isLocal(local) {
}
