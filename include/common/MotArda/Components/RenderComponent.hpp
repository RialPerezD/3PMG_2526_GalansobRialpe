#pragma once

#include <MotArda/Mesh.hpp>
#include <MotArda/Material.hpp>
#include <MotArda/ObjItem.hpp>

#include <string>
#include <vector>
#include <glad/glad.h>

#include <glm/glm.hpp>

namespace MTRD {
    class RenderComponent {
    public:
        std::shared_ptr<ObjItem> objitem_;

        RenderComponent();
        RenderComponent(std::shared_ptr<ObjItem> obj);
    };
}
