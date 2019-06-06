#include "GuiTeamMenu.h"

GuiTeamMenu::GuiTeamMenu()
{
	setColor(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	Input::setMouseVisible(true);


	auto mainContainer = Gui::create<GuiRect>(this);
	mainContainer->setPosition(vec2(0.1f, 0.1f));
	mainContainer->setColor(vec4(0.75f, 0.5f, 0.25, 1.0f));
	mainContainer->setSize(vec2(0.8f, 0.8f));

	label = Gui::create<GuiText>(mainContainer);
	label->setPosition(vec2(0.4f, 0.75f));
	label->setAlignment(TextAlign::CENTER);
	label->setText("Team Selection");
	label->setFont("Arial");

	message = Gui::create<GuiText>(mainContainer);
	message->setPosition(vec2(0.4f, 0.05f));
	message->setAlignment(TextAlign::CENTER);
	message->setText("");
	message->setFont("Arial");

	container_t1 = Gui::create<GuiRect>(mainContainer);
	container_t1->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	container_t1->setPosition(vec2(0.15f, 0.35f));
	container_t1->setSize(vec2(0.3f, 0.3f));

	container_t2 = Gui::create<GuiRect>(mainContainer);
	container_t2->setColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	container_t2->setPosition(vec2(0.4f, 0.35f));
	container_t2->setSize(vec2(0.3f, 0.3f));

	label_t1 = Gui::create<GuiText>(container_t1);
	label_t1->setPosition(vec2(0.06f, 0.1f));
	label_t1->setFont("Arial");

	label_t2 = Gui::create<GuiText>(container_t2);
	label_t2->setPosition(vec2(0.06f, 0.1f));
	label_t2->setFont("Arial");

	swtch = Gui::create<GuiButton>(mainContainer);
	swtch->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	swtch->setPosition(vec2(0.2f, 0.1f));
	swtch->setFont("Arial");
	swtch->setSize(vec2(0.5f, 0.2f));
	swtch->setText("switch");
	swtch->setAlignment(TextAlign::CENTER);
	auto onClick = std::bind(&GuiTeamMenu::handleSwitch, this);
	swtch->addCallback(onClick);

	//team1 = Gui::create<GuiText>(container_t1);
	//team1->setPosition(vec2(-0.1f, 0.1f));
	//team1->setAlignment(TextAlign::CENTER);
	//team1->setSize(vec2(1.0f, 0.1f));
	//team1->setText("");
	//team1->setFont("Arial");

	//team2 = Gui::create<GuiText>(container_t2);
	//team2->setPosition(vec2(0.0f, 0.1f));
	//team2->setAlignment(TextAlign::CENTER);
	//team2->setSize(vec2(1.0f, 0.1f));
	//team2->setText("");
	//team2->setFont("Arial");

	Network::on(NetMessage::TEAM, boost::bind(&GuiTeamMenu::updateTeamGui, this, _1, _2));
	Network::on(NetMessage::READY, boost::bind(&GuiTeamMenu::setReady, this, _1, _2));
	Network::on(NetMessage::START, boost::bind(&GuiTeamMenu::startGame, this, _1, _2));
}

void GuiTeamMenu::setPlayerId(int id) {
	this->playerId = id;
	label->setText("Team Selection: Player " + std::to_string(playerId));
}

void GuiTeamMenu::updateTeamGui(Connection *c, NetBuffer &buffer) {
	int size = buffer.read<int>();
	tuple<int, int> temp;
	int t;
	int p;
	float t1_pos = 0.1f;
	float t2_pos = 0.1f;

	id_name = game->getIdName();
	for (int i = 0; i < size; i++) {
		temp = buffer.read<tuple<int, int>>();
		player_team[std::get<0>(temp)] = std::get<1>(temp);
	}
	
	GuiText* label;

	for (auto it = team1.begin(); it != team1.end(); it++) {
		(*it)->remove();
	}

	for (auto it = team2.begin(); it != team2.end(); it++) {
		(*it)->remove();
	}

	team1.clear();
	team2.clear();

	for (auto it = player_team.begin(); it != player_team.end(); it++) {
		p = it->first;
		t = it->second;
		if (t == 0) {
			label = Gui::create<GuiText>(container_t1);
			label->setPosition(vec2(-0.1f, t1_pos));
			label->setAlignment(TextAlign::CENTER);
			label->setSize(vec2(1.0f, 0.1f));
			label->setText(id_name[p]);
			label->setFont("Arial");
			t1_pos += 0.1f;
			team1.push_back(label);
		}
		else {
			label = Gui::create<GuiText>(container_t2);
			label->setPosition(vec2(0.0f, t2_pos));
			label->setAlignment(TextAlign::CENTER);
			label->setSize(vec2(1.0f, 0.1f));
			label->setText(id_name[p]);
			label->setFont("Arial");
			t2_pos += 0.1f;
			team2.push_back(label);
		}
	}
}

void GuiTeamMenu::handleSwitch() {
	player_team[playerId] ^= 1;

	NetBuffer playerTeamSelection(NetMessage::TEAM);
	playerTeamSelection.write<int>(player_team.at(playerId));
	Network::connection->send(playerTeamSelection);
}

void GuiTeamMenu::setReady(Connection *c, NetBuffer &readyMsg) {
	bool ready = readyMsg.read<bool>();
	if (ready) {
		message->setText("Game is ready!");
		Input::setMouseVisible(false);
	}
	else {
		message->setText("The game needs 2 players on each team!");
	}
}

void GuiTeamMenu::startGame(Connection *c, NetBuffer &startMsg) {
	bool start = startMsg.read<bool>();
	if (start) {
		remove();
	}
}

bool GuiTeamMenu::getSelectionComplete() {
	return selectionComplete;
}

void GuiTeamMenu::setGame(Game *game) {
	this->game = game;
}