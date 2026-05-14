#pragma once
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>


namespace MTRD {
    class Material {
    public:
        std::string name;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
        float shininess;

        float metallic = 0.0f;
        float roughness = 0.5f;

        bool useHeightLUT = false;
        float maxHeight = 0.0f;

        bool loadeable;

        std::string diffuseTexPath;
        GLuint diffuseTexID;

        glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 uvScale = glm::vec2(1.0f, 1.0f);

        Material();
    };

}