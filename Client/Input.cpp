#include "Input.h"

GLFWwindow *curWindow;

bool Input::mouseLock = true;
std::vector<Input::KeyCallback> Input::keyCallbacks;
std::vector<Input::MouseButtonCallback> Input::mouseButtonCallbacks;

bool windowInFocus = true;

double lastMouseX = -1.0;
double lastMouseY = -1.0;
double mouseDeltaX = 0.0;
double mouseDeltaY = 0.0;

int keys[512] = { 0 };
int buttons[16] = { 0 };

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	keys[key] = action;
	for (auto &callback : Input::keyCallbacks) {
		callback(key, scancode, action, mods);
	}
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	buttons[button] = action;
	for (auto &callback : Input::mouseButtonCallbacks) {
		callback(button, action, mods);
	}
}

static void windowFocusCallback(GLFWwindow *window, int focused) {
	windowInFocus = focused;
}

void Input::init(GLFWwindow *window) {
	curWindow = window;
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetWindowFocusCallback(window, windowFocusCallback);
}

void Input::setMouseVisible(bool isVisible) {
	auto newMode = isVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN;
	glfwSetInputMode(curWindow, GLFW_CURSOR, newMode);
	mouseLock = !isVisible;
}

#include <iostream>
void Input::poll() {
	glfwPollEvents();

	if (!windowInFocus) {
		mouseDeltaX = mouseDeltaY = 0.0f;
		return;
	}

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
		mouseDeltaX = mouseDeltaY = 0;
		lastMouseX = mouseX;
		lastMouseY = mouseY;
	}
}

void Input::addKeyCallback(const KeyCallback &callback) {
	keyCallbacks.push_back(callback);
}

void Input::addMouseButtonCallback(const MouseButtonCallback &callback) {
	mouseButtonCallbacks.push_back(callback);
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
