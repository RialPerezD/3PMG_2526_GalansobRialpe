#include "MotArda/common/ObjLoader.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include "../deps/glm-master/glm/glm.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../deps/tiny_obj_loader.h"

namespace MTRD {

    std::optional<ObjLoader> ObjLoader::loadObj(
        const std::string& filepath,
        Window& window
    ) {
        tinyobj::ObjReader reader;
        std::string sourcePath = "../assets/";

        std::filesystem::path filePathObj(filepath);
        std::string filenameNoExt = filePathObj.stem().string();
        std::string objPath = sourcePath + "objs/" + filenameNoExt + "/" + filepath;

        if (!reader.ParseFromFile(objPath)) {
            std::cerr << "Error al cargar el archivo .obj: " << filepath << std::endl;
            return std::nullopt;
        }

        const tinyobj::attrib_t& attrib = reader.GetAttrib();
        const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
        const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

        if (!reader.Warning().empty()) {
            std::cout << "TinyObj Warning: " << reader.Warning() << std::endl;
        }

        ObjLoader objLoader;

        for (const auto& mat : materials) {
            Material material;
            material.name = mat.name;
            material.diffuse = glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
            material.specular = glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
            material.ambient = glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
            material.shininess = mat.shininess;

            std::filesystem::path p(mat.diffuse_texname);
            std::string texturePath = sourcePath + "textures/" + filenameNoExt + "/" + p.filename().string();
            material.diffuseTexPath = texturePath;

            material.loadeable = (mat.diffuse_texname.length() != 0);
            material.diffuseTexID = -1;

            objLoader.materials.push_back(material);
        }

        bool firstTime = true;
        for (const auto& shape : shapes) {
            std::vector<Vertex> vertexes;

            for (const auto& index : shape.mesh.indices) {
                Vertex vertex;
                vertex.position = glm::vec3(
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                );

                if (index.texcoord_index >= 0) {
                    vertex.uv = glm::vec2(
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    );
                }
                else {
                    vertex.uv = glm::vec2(0.0f, 0.0f);
                }

                if (index.normal_index >= 0) {
                    vertex.normal = glm::vec3(
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    );
                }
                else {
                    vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
                }

                vertexes.push_back(vertex);
            }

            int materialId = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];



            objLoader.meshes.emplace_back(Mesh::MeshUniquePtr(
                    vertexes,
                    window,
                    shape.name,
                    firstTime,
                    materialId,
                    true
                )
            );
            firstTime = false;
        }

        return objLoader;
    }

}