#pragma once

#include <GLFW/glfw3.h>

namespace Input {
	static bool mouseLock = true;

	void init(GLFWwindow *window);
	void poll();

	bool isKeyDown(int key);
	bool wasKeyPressed(int key);
	bool wasKeyReleased(int key);

	double getMouseX();
	double getMouseY();
	double getMouseDeltaX();
	double getMouseDeltaY();
};

