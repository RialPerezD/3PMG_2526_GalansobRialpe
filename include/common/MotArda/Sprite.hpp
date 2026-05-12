#pragma once

#include <MotArda/Mesh.hpp>
#include <MotArda/Material.hpp>

#include <string>
#include <vector>
#include "glad/glad.h"

#include "../../deps/glm-master/glm/glm.hpp"

namespace MTRD {
    class RenderComponent;

    class Sprite {
		friend class MotardaEng;

        Sprite(const char* spriteRoute, size_t ecsEntityIdentifyer_);

        float layer_;
        size_t ecsEntityIdentifyer_;
        RenderComponent* renderComponent_ = nullptr;

        int frameWidth_ = 0;
        int frameHeight_ = 0;
        int columns_ = 1;
        int rows_ = 1;
        int totalFrames_ = 1;
        int currentFrame_ = 0;

    public:
        Sprite(size_t ecsEntityIdentifyer, float layer);
        void setRenderComponent(RenderComponent* rc);

        size_t getId() { return ecsEntityIdentifyer_; }

        void setFrame(int frame);
        int getFrame() const { return currentFrame_; }
        int getTotalFrames() const { return totalFrames_; }
        void nextFrame();
        void setFrameSize(int width, int height);
    };
}