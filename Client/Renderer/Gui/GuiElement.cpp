#include "GuiElement.h"

void GuiElement::addChild(GuiElement *newChild) {
	if (newChild->getParent()) {
		newChild->getParent()->removeChild(newChild);
	}

	children.push_back(newChild);
	newChild->parent = this;
}

void GuiElement::removeChild(GuiElement *child) {
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end()) {
		(*it)->parent = nullptr;
		children.erase(it);
	}
}

void GuiElement::clearChildren() {
	for (auto child : children) {
		child->parent = nullptr;
	}
	children.clear();
}

GuiElement::Children &GuiElement::getChildren() {
	return children;
}

GuiElement *GuiElement::getParent() const {
	return parent;
}

void GuiElement::update(float dt) { }

void GuiElement::setPosition(const vec2 &newPosition) {
	position = newPosition;
}

const vec2 &GuiElement::getPosition() const {
	return position;
}

void GuiElement::setSize(const vec2 &newSize) {
	size = newSize;
}

const vec2 &GuiElement::getSize() const {
	return size;
}

void GuiElement::draw(float x, float y, float w, float h) const { }

void GuiElement::drawElement(const vec2 &offset) const {
	auto realPosition = position + offset;
	draw(realPosition.x, realPosition.y, size.x, size.y);
	for (auto &child : children) {
		child->drawElement(realPosition);
	}
}

GuiElement::~GuiElement() {
	for (auto it = children.begin(); it != children.end(); it++) {
		delete *it;
	}
	children.clear();
}