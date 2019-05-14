#include "Gui.h"

std::vector<GuiElement*> Gui::rootElements;

void Gui::onElementRemoved(GuiElement *element) {
	auto it = std::find(rootElements.begin(), rootElements.end(), element);
	if (it != rootElements.end()) {
		rootElements.erase(it);
	}
	delete element;
}

void Gui::onElementParentSet(GuiElement *element, GuiElement *parent) {
	// If there is a parent, it is no longer a root element.
	// If there is no parent, make sure rootElements won't have duplicates.
	auto it = std::find(rootElements.begin(), rootElements.end(), element);
	if (it != rootElements.end()) {
		if (parent) {
			rootElements.erase(it);
		}
		return;
	}

	// If no parent, then this is a root level element.
	if (!parent) {
		rootElements.push_back(element);
	}
}

void Gui::draw() {
	for (auto it = rootElements.begin(); it != rootElements.end(); it++) {
		(*it)->drawElement();
	}
}

void Gui::cleanUp() {
	for (auto it = rootElements.begin(); it != rootElements.end(); it++) {
		delete *it;
	}
	rootElements.clear();
}
