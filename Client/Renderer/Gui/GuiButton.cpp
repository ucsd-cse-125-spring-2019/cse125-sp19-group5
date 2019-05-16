#include "GuiButton.h"
#include "../Draw.h"

void GuiButton::setBgColor(const vec4 &newBgColor) {
	bgColor = newBgColor;
}

const vec4 &GuiButton::getBgColor() const {
	return bgColor;
}

void GuiButton::setHoverColor(const vec4 &newHoverColor) {
	hoverColor = newHoverColor;
}

const vec4 &GuiButton::getHoverColor() const {
	return hoverColor;
}

void GuiButton::setClickColor(const vec4 &newClickColor) {
	clickColor = newClickColor;
}

const vec4 &GuiButton::getClickColor() const {
	return clickColor;
}

void GuiButton::draw(float x, float y, float w, float h) const {
	Draw::setColor(bgColor);
	Draw::rect(x, y, w, h);

	if (isMouseHovering) {
		Draw::setColor(hoverColor);
		Draw::rect(x, y, w, h);
	}

	GuiText::draw(x, y, w, h);
}

void GuiButton::onMouseButton(float x, float y, int button, int action) {
	if (action == 1) {
		onMousePressed();
	} else if (action == 0) {
		onMouseReleased();
	}
}
