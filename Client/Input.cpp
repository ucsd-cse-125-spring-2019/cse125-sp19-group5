#include "Input.h"

GLFWwindow *curWindow;

double lastMouseX = -1.0;
double lastMouseY = -1.0;
double mouseDeltaX = 0.0;
double mouseDeltaY = 0.0;

int keys[512] = { 0 };
int buttons[16] = { 0 };

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	keys[key] = action;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	buttons[button] = action;
}

void Input::init(GLFWwindow *window) {
	curWindow = window;
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void Input::poll() {
	glfwPollEvents();

	double mouseX, mouseY;
	glfwGetCursorPos(curWindow, &mouseX, &mouseY);

	if (mouseX == -1.0) {
		lastMouseX = mouseX;
		lastMouseY = mouseY;
	}

	if (mouseLock) {
		int halfWidth, halfHeight;
		glfwGetWindowSize(curWindow, &halfWidth, &halfHeight);
		halfWidth /= 2;
		halfHeight /= 2;

		mouseDeltaX = mouseX - halfWidth;
		mouseDeltaY = mouseY - halfHeight;

		glfwSetCursorPos(curWindow, halfWidth, halfHeight);
	} else {
		mouseDeltaX = mouseX - lastMouseX;
		mouseDeltaY = mouseY - lastMouseY;

		lastMouseX = mouseX;
		lastMouseY = mouseY;
	}
}

bool Input::isKeyDown(int key) {
	return keys[key] > GLFW_RELEASE;
}

bool Input::wasKeyPressed(int key) {
	return keys[key] == GLFW_PRESS;
}

bool Input::wasKeyReleased(int key) {
	return keys[key] == GLFW_RELEASE;
}

double Input::getMouseX() {
	return lastMouseX;
}

double Input::getMouseY() {
	return lastMouseY;
}

double Input::getMouseDeltaX() {
	return mouseDeltaX;
}

double Input::getMouseDeltaY() {
	return mouseDeltaY;
}
