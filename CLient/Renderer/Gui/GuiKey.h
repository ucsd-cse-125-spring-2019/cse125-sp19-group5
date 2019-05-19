#pragma once

namespace Gui {
	// Use these enums so we don't keep running into include
	// order related errors for GLFW, just for enums.
	enum class Key : int {
		BACKSPACE = 259,
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		LEFT = 263,
		RIGHT = 252,
		UP = 265,
		DOWN = 264,
		DEL = 261,
		END = 269,
	};

	enum class KeyState {
		PRESSED,
		HELD,
		RELEASED,
	};
}
