#include "MotArda/common/Sprite.hpp"
#include <MotArda/common/Components/RenderComponent.hpp>

namespace MTRD {

    Sprite::Sprite(size_t ecsEntityIdentifyer, float layer) {
        ecsEntityIdentifyer_ = ecsEntityIdentifyer;
		layer_ = layer;
        currentFrame_ = 0;
        totalFrames_ = 1;
    }

    void Sprite::setRenderComponent(RenderComponent* rc) {
        renderComponent_ = rc;
    }

    void Sprite::setFrameSize(int width, int height) {
        frameWidth_ = width;
        frameHeight_ = height;
    }

    void Sprite::setFrame(int frame) {
        if (frame < 0 || frame >= totalFrames_) return;
        
        currentFrame_ = frame;
        
        if (renderComponent_ && renderComponent_->materials_ && !renderComponent_->materials_->empty()) {
            int col = currentFrame_ % columns_;
            int row = currentFrame_ / columns_;
            
            float uOffset = static_cast<float>(col * frameWidth_) / (frameWidth_ * columns_);
            float vOffset = static_cast<float>(row * frameHeight_) / (frameHeight_ * rows_);
            
            float uScale = static_cast<float>(frameWidth_) / (frameWidth_ * columns_);
            float vScale = static_cast<float>(frameHeight_) / (frameHeight_ * rows_);
            
            renderComponent_->materials_[0][0].uvOffset = glm::vec2(uOffset, vOffset);
            renderComponent_->materials_[0][0].uvScale = glm::vec2(uScale, vScale);
        }
    }

    void Sprite::nextFrame() {
        setFrame((currentFrame_ + 1) % totalFrames_);
    }
}