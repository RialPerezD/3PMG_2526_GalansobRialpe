#pragma once

#include <optional>
#include <vector>
#include <string>
#include "glad/glad.h"
#include "../deps/glm-master/glm/glm.hpp"
#include "Components/RenderComponent.hpp"
#include <GLFW/glfw3.h>

namespace MTRD {

	class Window; 

	/**
	* @class ObjLoader
	* @brief This class contains the functions to load an OBJ
	*/	
	class ObjLoader {
	public:
		/**
		* @brief loadObj
		* @param std::string& filepath Folder's path to the Obj
		* @return Returns an ObjLoader class' type object.
		* @details First, it receives the path to the OBJ, then it sets 
		* the object material and finally establishes the model or "shape", 
		* giving value to the vertices and normals.
		*/
		static std::optional<ObjLoader> loadObj(
			const std::string& filepath,
			const Window& parentWindow
		);

		/**
		* @brief getMeshes
		* @details Getter that returns a "Mesh" type object
		* return Returns meshes
		*/
		const std::vector<Mesh>& getMeshes() const { return meshes; }
		/**
		* @brief getMaterials
		* @details Getter that returns a "Material" type object
		* return Returns materials
		*/
		const std::vector<Material>& getMaterials() const { return materials; }

	private:
		//< Contains the data of the vertices.
		std::vector<Mesh> meshes;
		//< Will contain the data of OBJ's material 
		std::vector<Material> materials;
	};

}
