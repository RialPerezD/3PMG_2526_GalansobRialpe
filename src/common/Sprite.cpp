#include "MotArda/common/Sprite.hpp"

namespace MTRD {

    Sprite::Sprite(size_t ecsEntityIdentifyer, float layer) {
        ecsEntityIdentifyer_ = ecsEntityIdentifyer;
		layer_ = layer;
    }
}