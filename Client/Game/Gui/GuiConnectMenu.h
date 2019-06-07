#pragma once

#include <GL/glew.h>
#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiImage.h"
#include "../Renderer/Gui/GuiText.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/TextRenderer.h"
#include "../Game.h"

class GuiConnectMenu : public GuiImage {
	private:
	GuiText *label;
	GuiText *message;
	GuiTextbox *ipInput;
	GuiTextbox *nameInput;
	GuiButton *enter;

	void onIpEntered(const std::string &text);
	void handleEnter();

	Game * game = nullptr;

	public:
	GuiConnectMenu();
	void setGame(Game *game);
};

