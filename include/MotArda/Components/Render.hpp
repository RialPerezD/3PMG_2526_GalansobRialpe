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
        glm::mat4x4 model;

        Vertex();
    };


    struct Material {
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

    struct Shape {
        std::vector<Vertex> vertices;
        int materialId;
        GLuint vao;

        Shape();
    };


    struct Render {
        std::vector<Shape>* shapes;
        std::vector<Material>* materials;

        Render();
        Render(std::vector<Shape>* shps, std::vector<Material>* mats);
    };
}
