#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"
#include "../../Shared/Shared/CommonStructs.h"
#include "../../Networking/Client.h"
#include "../Game.h"

class MenuPrompt: public GuiRect {
	private:
		bool connected;
		int playerId;

		void onIpEntered(const std::string &text);
		void onPortEntered(const std::string &text);

		void handleMenuInput(const std::string &text);
		void handleMenuConfirmed();

		Game *game;
		GuiText *label;
		GuiText *label_t1;
		GuiText *label_t2;
		GuiText *message;
		GuiTextbox *ipInput;

	public:
		MenuPrompt();
		~MenuPrompt();
		void setGame(Game *game);
		bool getConnected();
		void ipPrompt();
		void settingsPrompt();
};
