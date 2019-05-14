#pragma once

#include "GuiElement.h"

class GuiText : public GuiElement {
	private:
	vec4 color = vec4(1.0f);
	std::string text;
	std::string font;
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

	void draw(float x, float y, float w, float h) const override;
};