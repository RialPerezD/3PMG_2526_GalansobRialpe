#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

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