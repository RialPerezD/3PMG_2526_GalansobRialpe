#pragma once
#include <cstdint>

namespace MTRD {

    struct NetworkComponent {
        uint32_t networkID;
        float meshId_;
        bool isLocal;

        NetworkComponent();
        NetworkComponent(uint32_t id, float meshId, bool local);
    };
}
