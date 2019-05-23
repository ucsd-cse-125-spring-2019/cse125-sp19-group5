#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../../Shared/Shared/CommonStructs.h"

class GuiConnectMenu : public GuiRect {
	private:
	GuiText *label;
	GuiTextbox *ipInput;

	void onIpEntered(const std::string &text);

	public:
	GuiConnectMenu();
};

