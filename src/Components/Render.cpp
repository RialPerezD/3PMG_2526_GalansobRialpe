#include "MotArda/Components/Render.hpp"

namespace MTRD {

    Vertex::Vertex() : position(0.0f), uv(0.0f), normal(0.0f) {}


    Material::Material()
        : name(""), diffuse(0.0f), specular(0.0f), ambient(0.0f),
        shininess(0.0f), loadeable(false), diffuseTexPath(""), diffuseTexID(GL_INVALID_INDEX) {}


    Shape::Shape() : vertices(), materialId(-1), vao(GL_INVALID_INDEX) {}


    Render::Render() : shapes(), materials() {}


    Render::Render(std::vector<Shape>* shps, std::vector<Material>* mats){
        shapes = shps;
        materials = mats;
    }
}
