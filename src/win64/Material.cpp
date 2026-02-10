#include "../include/MotArda/win64/Material.hpp"

namespace MTRD {
    Material::Material() {
        name = "";
        diffuse = glm::vec3(0.0f);
        specular = glm::vec3(0.0f);
        ambient = glm::vec3(0.0f);
        shininess = 0.0f;

        loadeable = false;

        diffuseTexPath = "";
        diffuseTexID = GL_INVALID_INDEX;
    }
}