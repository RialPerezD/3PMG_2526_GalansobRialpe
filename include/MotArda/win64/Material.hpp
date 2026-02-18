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

        bool loadeable;

        std::string diffuseTexPath;
        GLuint diffuseTexID;

        Material();
    };

}