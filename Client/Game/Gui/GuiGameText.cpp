#include "GuiGameText.h"
#include "../Renderer/Gui/Gui.h";

GuiGameText::GuiGameText() {
	setPosition(vec2(0.0f, 0.4f));
	setSize(vec2(1.0f, 0.2f));
	setFont("Arial");
	setAlignment(TextAlign::CENTER);
	setColor(vec4(1.0f));

	rect = Gui::create<GuiRect>(this);
	rect->setSize(vec2(1.0f));
	rect->setColor(vec4(0.0f, 0.0f, 0.0f, 0.3f));
}

void GuiGameText::draw(float x, float y, float w, float h) const {
	rect->draw(x, y, w, h);
	GuiText::draw(x, y, w, h);
}
