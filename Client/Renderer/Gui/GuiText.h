#pragma once

#include "GuiElement.h"

enum class TextAlign {
	LEFT,
	RIGHT,
	CENTER,
	TOP,
	BOTTOM
};

class GuiText : public GuiElement {
	protected:
	vec4 color = vec4(1.0f);
	std::string text;
	std::string font;
	TextAlign align;
	float scale = 1.0f;

	public:
	void setText(const std::string &newText);
	std::string getText() const;

	void setFont(const std::string &newFont);
	std::string getFont() const;

	void setScale(float newScale);
	float getScale() const;

	void setColor(const vec4 &newColor);
	const vec4 &getColor() const;

	void setAlignment(TextAlign newAlign);
	TextAlign getAlignment() const;

	void draw(float x, float y, float w, float h) const override;
};