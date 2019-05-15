#pragma once

#include <vector>

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
	void dispatchClick(float x, float y, int button, int action);
	void dispatchCursorPos(float x, float y);
	void dispatchKey(std::string c);
	void draw();
	void cleanUp();
}