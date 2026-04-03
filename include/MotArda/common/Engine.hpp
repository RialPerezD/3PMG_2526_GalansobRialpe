#pragma once

#include <optional>
#include "../win64/window.hpp"
#include "Input.hpp"
#include "JobSystem.hpp"
#include "ObjLoader.hpp"
#include <MotArda/win64/Systems/RenderLightsSystem.hpp>
#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Systems/RenderSystem.hpp>
#include "Camera.hpp"
#include "Sprite.hpp"
#include <MotArda/win64/Systems/RenderDefferredSystem.hpp>
#include <MotArda/win64/Systems/RenderPbrSystem.hpp>
#include "PhysxMaster.hpp"

namespace MTRD {

	/**
	* @class MotardaEng
	* @brief This class contains the functions to manage the engine related
	* parts, as the window or inputs.
	*/
	class MotardaEng {

	public:
		// --- Constructors and operators ---
		/**
		* @brief Destructor
		* @details Destructor of the engine class.
		* It is setted as defatult
		*/
		~MotardaEng() = default;

		/**
		* @brief Copy constructor
		* @param const MotardaEng& right Reference to an Engine type object
		* @details Copy contructor has been removed.
		*/
		MotardaEng(const MotardaEng& right) = delete;
		/**
		* @brief Copy assignment
		* @param const MotardaEng& right Reference to an Engine type object
		* @details Copy assignment has been removed.
		*/
		MotardaEng& operator=(const MotardaEng& right) = delete;

		/**
		* @brief Move constructor
		* @param const MotardaEng&& right Reference to an Engine type object
		* @details Move constructor is setted as default.
		*/
		MotardaEng(MotardaEng&& right) = default;
		/**
		* @brief Move assignment
		* @param const MotardaEng&& right Reference to an Engine type object
		* @details Move assignment is setted as default.
		*/
		MotardaEng& operator=(MotardaEng&& right) = default;

		static std::optional<MotardaEng> createEngine(
			int width = 800,
			int height = 600,
			const char* windowName = "Motarda default name");

		// --- *** ---

		// --- Functions ---
		void SetDebugMode(bool debug);

		/**
		* TODO write online documentation
		*
		OnlineSystem* ActivateOnlineMode(
			bool ImServer = true,
			const char* ip = "127.0.0.0",
			float port = 1234
		);*/

		bool windowShouldClose();
		/**
		* @brief windowGetTimer.
		* @return Window time in seconds.
		* @details Calls the "timer" function from the Window class.
		*/
		double windowGetTimer();
		/**
		* @brief windowEndFrame.
		* @details Calls the "swapBuffers" and "pollEvents" functions from the Window class.
		*/
		void windowEndFrame();
		/**
		* @brief windowSetErrorCallback.
		* @param void(*function)(int, const char*) Function to callback.
		* @details Sets the error callback by calling "setErrorCallback"
		* from the Window class.
		*/
		void windowSetErrorCallback(void(*function)(int, const char*));
		/**
		* @brief windowGetSizeRatio.
		* @return Return the size ratio.
		* @details Getter of the size ratio. The function calls "getSizeRatio"
		* from Window class.
		*/
		float windowGetSizeRatio();
		/**
		* @brief windowEndFrame.
		* @details Clears the buffer of the window by calling "openglViewportAndClear"
		*/
		void windowInitFrame();
		/**
		* @brief windowLoadAllMaterials.
		* @param std::vector<MTRD::Window::ObjItem>& objItemsList List that
		* contains the shapes and materials of the objects.
		* @details Calls windowLoadMaterials() inside a for loop in order
		* to load the materials for all the objects.
		*/
		void windowLoadAllMaterials(std::vector<ObjItem>& objItemsList);
		/**
		* @brief windowGetLastFrameTime.
		* @details Manage the time in seconds by calling "getLastFrameTime" from
		* the window class.
		*/
		float windowGetLastFrameTime();

		/**
		* @brief inputIsKeyPressed.
		* @param Input::Keyboard key Pressed input by the user.
		* @details Register when the key is pressed.
		*/
		bool inputIsKeyPressed(Input::Keyboard key);
		/**
		* @brief inputIsKeyDown.
		* @param Input::Keyboard key Pressed input by the user.
		* @details Register if the key is being pressed (in case the
		* user holds it).
		*/
		bool inputIsKeyDown(Input::Keyboard key);
		/**
		* @brief inputIsKeyUp.
		* @param Input::Keyboard key Pressed input by the user.
		* @details Register the exact moment when the input is released.
		*/
		bool inputIsKeyUp(Input::Keyboard key);

		/**
		* @brief enqueueTask.
		* @param std::function<void()> task Future function to be processed
		* by workers
		* @details Adds a task to the queue by blocking the mutex and waking up
		* a thread.
		*/
		void enqueueTask(std::function<void()> task);

		/**
		* @brief loadObjs.
		* @param std::vector <const char*> routes Path of the OBJs to load.
		* @return Returns a list with the OBJs to load.
		* @details First, a list of objects is created, and then, the routes are loaded.
		* Next, a new ObjItem type varaible is created to store the shapes and materials,
		* and finally, we move "item" into the first list of objects.
		*/
		std::vector<ObjItem> loadObjs(std::vector <const char*> routes);
		/**
		* @brief loadObjs.
		* @param const char* filename Name of the shader.
		* @return Returns a pointer containing the shader source code.
		* @details Loads the contents of a shader file and returns it as a string.
		*/
		const char* loadShaderFile(const char* filename);

		std::unique_ptr<Mesh> createMesh(std::vector<Vertex> vertices, std::string name);

		ObjItem generateCube(float size, int texureId = 0, bool debug = true);
		ObjItem generatePlane(float width, float height, int texureId = 0, bool debug = true);
		ObjItem generateSphere(float radius, int segments, int rings, int texureId = 0, bool debug = true);

		// TODO , poner en la documentacion que esto inserta un transform y un render components por defecto
		Sprite& generateSprite(GLuint textureIndex, float size, float deep = 0);
		Sprite& generateSpriteSheet(GLuint textureIndex, float size, int frameWidth, int frameHeight, int columns, int rows, float deep = 0);
		//------------

		// --- Utility functions ---
		Camera& getCamera() { return camera_; }
		ECSManager& getEcs() { return ecs_; }
		void createPhysxActor(MTRD::PhysxComponent& p, MTRD::TransformComponent& t);
		void hasPhysx(bool has);

		// --- *** ---

		// --- Render Information ---
		enum RenderType {
			Base,
			Lights,
			LightsWithShadows,
			Bidimensional,
			DeferredWithLights,
			Pbr
		};

		void SetRenderType(RenderType type);

		void RenderScene();

		//------------

	private:
		/**
		* @brief Constructor
		* @param Window window Window class-type object.
		* @param Input input Input class-type object.
		* @param JobSystem js JobSystem class-type object
		* @details Constructor of the engine class.
		*/
		MotardaEng(Window window, Input input, JobSystem js);

		bool debug_;
		bool online_;

		//< Object of Window class.
		Window window_;
		//< Object of Input class.
		Input input_;
		//< Object of JobSystem class.
		JobSystem jobSystem_;

		Camera camera_;
		ECSManager ecs_;

		// --- Online Information ---
		//OnlineSystem onlineSystem_;

		// --- Render Information ---
		glm::mat4x4 vp_;
		glm::mat4x4 model_;

		std::unique_ptr<RenderSystem> renderSystem_;
		std::unique_ptr<RenderLightsSystem> renderLightsSystem_;
		std::unique_ptr<ShadowMapSystem> shadowSystem_;
		std::unique_ptr<RenderDefferredSystem> defferredSystem_;
		std::unique_ptr<RenderPbrSystem> pbrSystem_;
		RenderType actualRenderType_ = RenderType::Base;

		bool initialized2D;
		ObjItem basePlane_;

		bool hasPhysx_;
		PhysxMaster physx_;
		// --- *** ---
	};
}