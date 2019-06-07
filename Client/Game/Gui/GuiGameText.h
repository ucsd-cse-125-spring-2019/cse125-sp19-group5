#pragma once
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"

class GuiGameText : public GuiText {
	public:
	GuiGameText();
	void draw(float x, float y, float w, float h) const override;

	private:
	GuiRect *rect;
};