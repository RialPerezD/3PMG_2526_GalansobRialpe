#include "MotArda/Components/Render.hpp"

namespace MTRD {

    Vertex::Vertex() : position(0.0f), uv(0.0f), normal(0.0f) {}


    Material::Material()
        : name(""), diffuse(0.0f), specular(0.0f), ambient(0.0f),
        shininess(0.0f), diffuseTexPath(""), diffuseTexID(0) {}


    Render::Render() : vertices(), materials() {}


    Render::Render(std::vector<Vertex>* vrts, std::vector<Material>* mats){
        vertices = vrts;
        materials = mats;
    }
}
