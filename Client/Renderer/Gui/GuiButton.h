#pragma once

#include "GuiText.h"
#include <functional>

class GuiButton : public GuiText {
	private:
	std::vector<std::function<void()>> callbacks;
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

	void addCallback(const std::function<void()> callback);

	void onMouseButton(float x, float y, int button, int action) override;
	virtual void onMousePressed();
	virtual void onMouseReleased() { };
};