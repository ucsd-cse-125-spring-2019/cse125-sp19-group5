#pragma once

#include <vector>
#include <string>
#include "GuiKey.h"
#include "../Input.h"
#include <GLFW/glfw3.h>

class GuiElement;
#include "GuiElement.h"


namespace Gui {
	extern std::vector<GuiElement*> rootElements;
	extern std::vector<GuiElement*> allElements;

	template<class T>
	T* create(GuiElement *parent = nullptr) {
		auto element = new T;

		if (parent) {
			parent->addChild(element);
		} else {
			rootElements.push_back(element);
		}
		allElements.push_back(element);

		return element;
	}

	void onElementParentSet(GuiElement *element, GuiElement *parent);
	void onElementRemoved(GuiElement *element);
	void setupInputListeners(GLFWwindow *window);
	void dispatchMouseButton(float x, float y, int button, int action);
	void dispatchMousePos(float x, float y);
	void dispatchChar(const std::string &c);
	void dispatchKey(Key key, bool pressed);
	void draw();
	void update(float dt);
	void cleanUp();
}