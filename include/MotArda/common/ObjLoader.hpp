#pragma once

#include <optional>
#include <vector>
#include <string>
#include <memory>
#include "glad/glad.h"
#include "../deps/glm-master/glm/glm.hpp"
#include <MotArda/win64/Mesh.hpp>
#include <MotArda/win64/Material.hpp>

namespace MTRD {

    class Window;

    class ObjLoader {
    public:
        static std::optional<ObjLoader> loadObj(
            const std::string& filepath,
            Window& window
        );

        std::vector<Mesh> getMeshes() { return std::move(meshes); }
        std::vector<Material> getMaterials() { return std::move(materials); }

    private:
        std::vector<Mesh> meshes;
        std::vector<Material> materials;
    };

}