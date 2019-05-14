#include "GuiText.h"
#include "../TextRenderer.h"

void GuiText::setText(const std::string &newText) {
	text = newText;
}

std::string GuiText::getText() const {
	return text;
}

void GuiText::setFont(const std::string &newFont) {
	font = newFont;
}

std::string GuiText::getFont() const {
	return font;
}

void GuiText::setScale(float newScale) {
	scale = newScale;
}

float GuiText::getScale() const {
	return scale;
}

void GuiText::setColor(const vec4 &newColor) {
	color = newColor;
}

const vec4 &GuiText::getColor() const {
	return color;
}

void GuiText::draw(float x, float y, float w, float h) const {
	gTextRenderer->renderText(text, font, x, y, color, scale);
}