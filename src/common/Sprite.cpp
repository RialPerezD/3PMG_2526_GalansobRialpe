#include "MotArda/common/Sprite.hpp"

namespace MTRD {
    Sprite::Sprite(const char* spriteRoute, size_t ecsEntityIdentifyer_):
        ecsEntityIdentifyer_(ecsEntityIdentifyer_),
        layer_ ( -1 )
    {}


    Sprite Sprite::GenerateSprite(const char* spriteRoute, size_t ecsEntityIdentifyer_) {
        return Sprite(spriteRoute, ecsEntityIdentifyer_);
    }
}