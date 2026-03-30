#pragma once
#include <cstdint>

namespace MTRD {

    struct NetworkComponent {
        uint32_t networkID;
        bool isLocal;

        NetworkComponent();
        NetworkComponent(uint32_t id, bool local);
    };
}
