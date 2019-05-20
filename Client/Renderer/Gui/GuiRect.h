#pragma once

#include "GuiElement.h"

class GuiRect : public GuiElement {
	public:
	void draw(float x, float y, float w, float h) const override;

	void setColor(const vec4 &newColor);
	const vec4 &getColor() const;

	protected:
	vec4 color = vec4(1.0f);
};