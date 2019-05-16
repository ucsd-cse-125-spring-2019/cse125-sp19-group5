#include "GuiText.h"
#include "../TextRenderer.h"
#include "../Draw.h"

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

void GuiText::setAlignment(TextAlign newAlign) {
	align = newAlign;
}

TextAlign GuiText::getAlignment() const {
	return align;
}

void GuiText::draw(float x, float y, float w, float h) const {
	auto offset = vec2(0.0f);
	auto size = gTextRenderer->getTextSize(text, font, scale);
	size.x /= Draw::screenWidth;
	size.y /= Draw::screenHeight;

	switch (align) {
		case TextAlign::TOP:
			offset.y = size.y * 0.5f;
			break;
		case TextAlign::BOTTOM:
			offset.y = size.y * -0.5f;
			break;
		case TextAlign::RIGHT:
			offset.x = w - size.x;
			break;
		case TextAlign::CENTER:
			offset = vec2(w - size.x, h - size.y) * 0.5f;
			break;
	}

	gTextRenderer->renderText(
		text,
		font,
		x + offset.x,
		y + offset.y,
		color,
		scale
	);
}