#pragma once

#include <optional>
#include "../win64/window.hpp"
#include "Input.hpp"
#include "JobSystem.hpp"
#include "ObjLoader.hpp"

namespace MTRD {

	/**
	* @class MotardaEng
	* @brief This class contains the functions to manage the engine related
	* parts, as the window or inputs.
	*/
	class MotardaEng {

	public:
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

		/**
		* @brief windowShouldClose.
		* @details Calls the "shouldClose" function from the Window class.
		*/
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
		* @brief windowOpenglSetUniformsValues.
		* @param std::vector<Window::UniformAttrib> uniforms Uniforms for the window.
		* @details Sets all the uniforms of the window by calling "openglSetUniformsValues"
		*/
		void windowOpenglSetUniformsValues(std::vector<Window::UniformAttrib> uniforms);
		/**
		* @brief windowLoadAllMaterials.
		* @param std::vector<MTRD::Window::ObjItem>& objItemsList List that
		* contains the shapes and materials of the objects.
		* @details Calls windowLoadMaterials() inside a for loop in order
		* to load the materials for all the objects.
		*/
		void windowLoadAllMaterials(std::vector<MTRD::Window::ObjItem>& objItemsList);
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
		std::vector<MTRD::Window::ObjItem> loadObjs(std::vector <const char*> routes);
		/**
		* @brief loadObjs.
		* @param const char* filename Name of the shader.
		* @return Returns a pointer containing the shader source code.
		* @details Loads the contents of a shader file and returns it as a string.
		*/
		const char* loadShaderFile(const char* filename);

		static std::optional<MotardaEng> createEngine(
			int width = 800,
			int height = 600,
			const char* windowName = "Motarda default name");

	private:
		/**
		* @brief Constructor
		* @param Window window Window class-type object.
		* @param Input input Input class-type object.
		* @param JobSystem js JobSystem class-type object
		* @details Constructor of the engine class.
		*/
		MotardaEng(Window window, Input input, JobSystem js);
		//< Object of Window class.
		Window window_;
		//< Object of Input class.
		Input input_;
		//< Object of JobSystem class.
		JobSystem jobSystem_;

		/**
		* @brief windowLoadMaterials
		* @param std::vector<Material>& materials Vector of materials for
		* the OBJs.
		* @details Load all materials by calling "openglLoadMaterials".
		*/
		void windowLoadMaterials(std::vector<Material>& materials);
	};
}