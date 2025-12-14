#pragma once

#include <optional>
#include <vector>
#include <map>
#include "MotArda/window.hpp"


namespace MTRD {

    /**
    * @class Input
    * @brief Manages the inputs of the engine.
    */
	class Input {
	public:
        enum class Keyboard {
            A,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z
        };
        
        /**
        * @brief Creates an Input type object.
        * @param Window& window Reference to a Window type object.
        * @details Register a new Input.
        */
		static Input inputCreate(Window& window);

        /**
        * @brief Destructor
        * @details 
        */
		~Input();

		//--Dissable copy constructor and enable noving constructors--

        /**
        * @brief Copy constructor.
        * @param Input& right Reference to an Input type object.
        * @details We set it to delete because there is no need to copy anything.
        */
		Input(const Input& right) = delete;
        /**
        * @brief Copy assignment.
        * @param Input& right Reference to an Input type object.
        * @details We set it to delete because there is no need to copy anything.
        */
		Input& operator=(const Input& right) = delete;

        /**
        * @brief Move constructor.
        * @param Input&& right Reference to an Input type object.
        * @details --------------
        */
		Input(Input&& right) noexcept;
		Input& operator=(Input&& right) = default;

		//------------Functions-----------------------

        /**
        * @brief Calls the setKeyCallback function.
        * @param Keyboard The engine will check if the 
        * key matches with user's pressed one.
        * @details ------------
        */
		bool isKeyPressed(Keyboard);
        bool isKeyDown(Keyboard);
        bool isKeyUp(Keyboard);

        void generateAsciiMap();
        /**
        * @brief Calls the setKeyCallback function.
        * @param Window& window Reference to a Window type object.
        * @details It allows the input events to happen.
        */
        void setKeyboardCallback(Window& window);
        /**
        * @brief Release the vectors.
        * @details Clears the pressKey and releaseKey vectors.
        */
        void clearBuffers();

        //< Key pressed by the user.
        static std::vector<int> pressKey;
        //< Key being holded by the user.
		static std::vector<int> repeatKey;
        //< Key released this frame by the user.
        static std::vector<int> releaseKey;


	private:
        /**
        * @brief Constructor
        * @details Clears repeatKey and calls a function
        * in order to clear all buffers.
        */
        Input();
        //< Map that matches a key with their ascii number
        std::map<Keyboard, std::vector<int>> asciiMap;
	};
}