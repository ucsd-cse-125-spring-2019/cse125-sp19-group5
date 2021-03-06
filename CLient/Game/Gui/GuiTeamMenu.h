#pragma once

#include <GL/glew.h>
#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiImage.h"
#include "../Renderer/Gui/GuiText.h"
#include "../../Shared/Shared/CommonStructs.h"
#include "../../Networking/Client.h"
#include "../Game.h"

#include <iostream>

class GuiTeamMenu : public GuiImage {
private:
	bool selectionComplete;
	bool isReady = false;
	int playerId;

	void handleSwitch();
	void handleReady();

	GuiText *label;
	GuiText *label_t1;
	//GuiText *team1;
	GuiText *label_t2;
	//GuiText *team2;
	GuiText *message;
	GuiButton *swtch;
	GuiButton *ready;
	GuiRect *container_t1;
	GuiRect *container_t2;

	Game *game = nullptr;
	unordered_map<int, int> player_team;
	unordered_map<int, std::string> id_name;
	vector<GuiText*> team1;
	vector<GuiText*> team2;

public:
	GuiTeamMenu();
	void setPlayerId(int id);
	bool getSelectionComplete();
	void setGame(Game *game);
	void updateTeamGui(Connection *c, NetBuffer &menuMsg);
	void setReady(Connection *c, NetBuffer &readyMsg);
	void startGame(Connection *c, NetBuffer &readyMsg);
	void setPlayerTeam(unordered_map<int, int> &p_t);
	void resetGui();
};