#pragma once

#include <GLFW/glfw3.h>

namespace Gui {
	// Use these enums so we don't keep running into include
	// order related errors for GLFW, just for enums.
	enum class Key : int {
		BACKSPACE = GLFW_KEY_BACKSPACE,
		ESCAPE = GLFW_KEY_ESCAPE,
		ENTER = GLFW_KEY_ENTER,
		TAB = GLFW_KEY_TAB,
		LEFT = GLFW_KEY_LEFT,
		RIGHT = GLFW_KEY_RIGHT,
		UP = GLFW_KEY_UP,
		DOWN = GLFW_KEY_DOWN,
		DELETE = GLFW_KEY_DELETE,
		END = GLFW_KEY_END,
	};
}
