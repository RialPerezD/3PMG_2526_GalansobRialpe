#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>
#include "MotArda/common/ObjItem.hpp"

namespace MTRD {
    class RenderComponent {
    public:
        std::shared_ptr<ObjItem> objitem_;

        RenderComponent();
        RenderComponent(std::shared_ptr<ObjItem> obj);
    };
}
