#pragma once
#include <GL/glew.h>
#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"
#include "../../Shared/Shared/CommonStructs.h"
#include "../../Networking/Client.h"
#include "../Game.h"

#include <iostream>
using std::string;

class GuiTeamMenu : public GuiRect {
private:
	bool selectionComplete;
	int playerId;

	void handleSwitch();

	GuiText *label;
	GuiText *label_t1;
	GuiText *team1;
	GuiText *label_t2;
	GuiText *team2;
	GuiText *message;
	GuiButton *swtch;

	Game*game;
	unordered_map<int, int> player_team;
	unordered_map<int, std::string> id_name;

public:
	GuiTeamMenu();
	void setPlayerId(int id);
	void updateTeamGui(Connection *c, NetBuffer &menuMsg);
	void setReady(Connection *c, NetBuffer &readyMsg);
	bool getSelectionComplete();
	void setGame(Game *game);
};