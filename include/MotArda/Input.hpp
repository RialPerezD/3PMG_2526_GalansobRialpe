#pragma once

#include <optional>
#include <vector>
#include <map>
#include "MotArda/window.hpp"


namespace MTRD {

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

		static Input inputCreate(Window& window);

        //TODO
		~Input();

		//--Dissable copy constructor and enable noving constructors--
		Input(const Input& right) = delete;
		Input& operator=(const Input& right) = delete;

		Input(Input&& right);
		Input& operator=(Input&& right) = default;

		//------------Functions-----------------------

		void clearInputs();
		bool isKeyPressed(Keyboard);
        void generateAsciiMap();
        void setKeyboardCallback(Window& window);
		static std::vector<int> pressedKey;


	private:
        Input();
        std::map<Keyboard, std::vector<int>> asciiMap;
	};
}