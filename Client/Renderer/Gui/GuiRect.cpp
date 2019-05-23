#include "GuiRect.h"
#include "../Draw.h"

void GuiRect::draw(float x, float y, float w, float h) const {
	Draw::setColor(color);
	Draw::rect(x, y, w, h);
}

void GuiRect::setColor(const vec4 &newColor) {
	color = newColor;
}

const vec4 &GuiRect::getColor() const {
	return color;
}