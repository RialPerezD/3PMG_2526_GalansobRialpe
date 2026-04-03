#pragma once
#include <string>
#include "../deps/glm-master/glm/glm.hpp"
#include <glad/glad.h>


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

        bool loadeable;

        std::string diffuseTexPath;
        GLuint diffuseTexID;

        glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 uvScale = glm::vec2(1.0f, 1.0f);

        Material();
    };

}