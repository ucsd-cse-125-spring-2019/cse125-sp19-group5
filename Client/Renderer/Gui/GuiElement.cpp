#include "GuiElement.h"

#include "../Draw.h"
#include "Gui.h"

void GuiElement::addChild(GuiElement *newChild) {
	if (newChild->getParent()) {
		newChild->getParent()->removeChild(newChild);
	}

	children.push_back(newChild);
	newChild->parent = this;
	Gui::onElementParentSet(newChild, this);
}

void GuiElement::removeChild(GuiElement *child) {
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end()) {
		auto child = *it;
		child->parent = nullptr;
		children.erase(it);
		Gui::onElementParentSet(child, nullptr);
	}
}

void GuiElement::clearChildren() {
	for (auto child : children) {
		child->parent = nullptr;
		Gui::onElementParentSet(child, nullptr);
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
	auto realSize = size;
	if (parent) {
		realSize *= parent->getSize();
	}
	draw(realPosition.x, realPosition.y, realSize.x, realSize.y);
	for (auto &child : children) {
		child->drawElement(realPosition);
	}
}

void GuiElement::remove() {
	if (parent) {
		parent->removeChild(this);
	}
	Gui::onElementRemoved(this);
}

GuiElement::~GuiElement() {
	for (auto it = children.begin(); it != children.end(); it++) {
		delete *it;
	}
	children.clear();
}

vec2 GuiElement::getScreenPos(const vec2 &pos) const {
	if (parent) {
		return parent->getScreenPos(position) + pos;
	}
	return position + pos;
}

vec2 GuiElement::getScreenSize() const {
	vec2 absSize = size;
	auto curParent = parent;
	while (curParent) {
		absSize *= curParent->getSize();
		curParent = curParent->getParent();
	}
	return absSize;
}

bool GuiElement::isAbsPosWithin(float posX, float posY) const {
	auto absPos = getScreenPos();
	auto absSize = getScreenSize();
	return posX >= absPos.x
		&& posY >= absPos.y
		&& posX <= (absPos.x + absSize.x)
		&& posY <= (absPos.y + absSize.y);
}

bool GuiElement::dispatchKey(const std::string &key) {
	for (auto child : children) {
		if (child->dispatchKey(key)) {
			return true;
		}
	}
	onKeyPressed(key);
	return false;
}

bool GuiElement::dispatchMousePos(float x, float y) {
	if (children.empty()) {
		auto mouseInside = isAbsPosWithin(x, y);
		if (mouseInside && !isMouseHovering) {
			isMouseHovering = true;
			onMouseEntered(x, y);
		} else if (!mouseInside && isMouseHovering) {
			isMouseHovering = false;
			onMouseLeft(x, y);
		}
		return true;
	}

	for (auto child : children) {
		if (child->dispatchMousePos(x, y)) {
			return true;
		}
	}

	return false;
}

bool GuiElement::dispatchMouseButton(float x, float y, int button, int action) {
	if (isMouseHovering) {
		onMouseButton(x, y, button, action);
		return true;
	}

	for (auto child : children) {
		if (child->dispatchMouseButton(x, y, button, action)) {
			return true;
		}
	}

	return false;
}

void GuiElement::update(float dt) { }