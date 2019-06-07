#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

enum CursorShape {
	ARROW = 0x00036001,
	BEAM = 0x00036002,
	HAND = 0x00036004,
};


namespace Input {
	using KeyCallback = std::function<void(int, int, int, int)>;
	using MouseButtonCallback = std::function<void(int, int, int)>;

	extern bool mouseLock;
	extern std::vector<KeyCallback> keyCallbacks;
	extern std::vector<MouseButtonCallback> mouseButtonCallbacks;

	void setMouseVisible(bool isVisible = true);
	void init(GLFWwindow *window);
	void poll();
	void addKeyCallback(const KeyCallback &callback);
	void addMouseButtonCallback(const MouseButtonCallback &callback);

	bool isKeyDown(int key);
	bool wasKeyPressed(int key);
	bool wasKeyReleased(int key);
	bool isMouseClicked(int button);

	double getMouseX();
	double getMouseY();
	double getMouseDeltaX();
	double getMouseDeltaY();
};

