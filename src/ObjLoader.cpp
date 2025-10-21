#include "MotArda/ObjLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/tinyobjloader-release/tiny_obj_loader.h"
#include "../deps/tinyobjloader-release/tiny_obj_loader.cc"

namespace MTRD {

	std::optional<ObjLoader> ObjLoader::loadObj(const std::string& filepath) {
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = ""; 

		if (!reader.ParseFromFile(filepath, reader_config)) {
			std::cerr << "Error al cargar el archivo .obj: " << filepath << std::endl;
			return std::nullopt;
		}

		const tinyobj::attrib_t& attrib = reader.GetAttrib();
		const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
		const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

		ObjLoader objLoader;

		for (const auto& shape : shapes) {
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
				} else {
					vertex.uv = glm::vec2(0.0f, 0.0f);
				}

				if (index.normal_index >= 0) {
					vertex.normal = glm::vec3(
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					);
				} else {
					vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
				}

				objLoader.vertices.push_back(vertex);
			}
		}

		return objLoader;
	}

}
