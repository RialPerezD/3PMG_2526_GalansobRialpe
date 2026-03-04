#include "../include/MotArda/win64/Material.hpp"

namespace MTRD {
    Material::Material() {
        name = "DefaultText";
        diffuse = glm::vec3(1.0f);
        specular = glm::vec3(1.0f);
        ambient = glm::vec3(0.2f);
        shininess = 32.0f;

        loadeable = true;

        diffuseTexPath = "";
        diffuseTexID = GL_INVALID_INDEX;
    }
}