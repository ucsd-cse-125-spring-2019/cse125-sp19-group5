#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"

class GuiConnectMenu : public GuiRect {
	private:
	GuiText *label;
	GuiText *message;
	GuiTextbox *ipInput;
	GuiTextbox *nameInput;
	GuiButton *enter;

	void onIpEntered(const std::string &text);
	void handleEnter();

	public:
	GuiConnectMenu();
};

