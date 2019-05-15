#pragma once

#include "GuiText.h"

class GuiButton : public GuiText {
	private:
	vec4 bgColor = vec4(1.0f);
	vec4 hoverColor = vec4(1.0f, 1.0f, 1.0f, 0.1f);
	vec4 clickColor = vec4(0.0f, 0.0f, 0.0f, 0.1f);

	public:
	void setBgColor(const vec4 &newBgColor);
	const vec4 &getBgColor() const;

	void setHoverColor(const vec4 &newHoverColor);
	const vec4 &getHoverColor() const;

	void setClickColor(const vec4 &newClickColor);
	const vec4 &getClickColor() const;

	void draw(float x, float y, float w, float h) const override;
};