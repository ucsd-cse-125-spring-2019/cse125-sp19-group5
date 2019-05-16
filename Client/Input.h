#pragma once

#include <GLFW/glfw3.h>

enum CursorShape {
	ARROW = 0x00036001,
	BEAM = 0x00036002,
	HAND = 0x00036004,
};

namespace Input {
	extern bool mouseLock;

	void setMouseVisible(bool isVisible = true);
	void init(GLFWwindow *window);
	void poll();
	void setCursorShape(CursorShape shape);

	bool isKeyDown(int key);
	bool wasKeyPressed(int key);
	bool wasKeyReleased(int key);

	double getMouseX();
	double getMouseY();
	double getMouseDeltaX();
	double getMouseDeltaY();
};

