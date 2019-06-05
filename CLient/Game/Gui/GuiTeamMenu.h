#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"
#include "../../Shared/Shared/CommonStructs.h"
#include "../../Networking/Client.h"
#include "../Game.h"

class GuiTeamMenu : public GuiRect {
private:
	bool selectionComplete;
	int playerId;

	void handleSwitch();

	Game *game;
	GuiText *label;
	GuiText *label_t1;
	GuiText *team1;
	GuiText *label_t2;
	GuiText *team2;
	GuiText *message;
	GuiButton *swtch;

	unordered_map<int, int> player_team;

public:
	GuiTeamMenu();
	void setGame(Game *game);
	void setPlayerId(int id);
	void updateTeamGui(Connection *c, NetBuffer &menuMsg);
	void setReady(Connection *c, NetBuffer &readyMsg);
	bool getSelectionComplete();
};