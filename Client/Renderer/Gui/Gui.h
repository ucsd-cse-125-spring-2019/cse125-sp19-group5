#pragma once

#include <vector>
#include <string>
#include <GLFW/glfw3.h>

class GuiElement;
#include "GuiElement.h"

namespace Gui {
	extern std::vector<GuiElement*> rootElements;

	template<class T>
	T* create(GuiElement *parent = nullptr) {
		auto element = new T;

		if (parent) {
			parent->addChild(element);
		} else {
			rootElements.push_back(element);
		}

		return element;
	}

	void onElementParentSet(GuiElement *element, GuiElement *parent);
	void onElementRemoved(GuiElement *element);
	void setupInputListeners(GLFWwindow *window);
	void dispatchMouseButton(float x, float y, int button, int action);
	void dispatchMousePos(float x, float y);
	void dispatchKey(const std::string &key);
	void draw();
	void cleanUp();
}