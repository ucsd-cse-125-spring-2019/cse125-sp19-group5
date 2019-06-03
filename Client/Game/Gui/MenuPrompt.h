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
		bool selectionComplete;
		int playerId;

		void handleMenuInput(const std::string &text);
		void handleMenuConfirm(Connection *s, NetBuffer &menuMsg);
		void handlePickAgain(Connection *s, NetBuffer &menuMsg);

		Game *game;
		GuiText *label;
		GuiText *label_t1;
		GuiText *team1;
		GuiText *label_t2;
		GuiText *team2;
		GuiText *message;
		GuiTextbox *ipInput;

	public:
		MenuPrompt();
		void updateTeamGui();
		void setGame(Game *game);
		void setPlayerId(int id);
		void settingsPrompt();
		bool getSelectionComplete();
};
