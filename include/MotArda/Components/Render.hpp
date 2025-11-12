#pragma once
#include <string>
#include <vector>
#include "../deps/glm-master/glm/glm.hpp"
#include "glad/glad.h"

namespace MTRD {

    struct Vertex {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;

        Vertex();
    };


    struct Material {
        std::string name;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
        float shininess;

        std::string diffuseTexPath;
        GLuint diffuseTexID;

        Material();
    };


    struct Render {
        std::vector<Vertex>* vertices;
        std::vector<Material>* materials;

        Render();
        Render(std::vector<Vertex>* vrts, std::vector<Material>* mats);
    };
}
