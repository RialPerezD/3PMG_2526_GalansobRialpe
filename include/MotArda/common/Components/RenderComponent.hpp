#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

namespace MTRD {
    class RenderComponent {
    public:
        std::vector<Mesh>* meshes_;
        std::vector<Material>* materials_;

        RenderComponent();
        RenderComponent(std::vector<Mesh>* meshes, std::vector<Material>* mats);
    };
}
