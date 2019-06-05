#include "GuiTeamMenu.h"

GuiTeamMenu::GuiTeamMenu()
{

	setColor(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	Input::setMouseVisible(true);

	message = Gui::create<GuiText>(this);
	message->setPosition(vec2(0.0f, 0.8f));
	message->setColor(vec4(1.0f, 0.1f, 0.1, 1.0f));
	message->setText("");
	message->setSize(vec2(1.0f, 0.1f));
	message->setFont("Arial");
	message->setAlignment(TextAlign::CENTER);

	label = Gui::create<GuiText>(this);
	label->setPosition(vec2(0.0f, 0.7f));
	label->setAlignment(TextAlign::CENTER);
	label->setSize(vec2(1.0f, 0.1f));
	label->setText("Team Selection");
	label->setFont("Arial");

	label_t1 = Gui::create<GuiText>(this);
	label_t1->setPosition(vec2(0.3f, 1.0f));
	label_t1->setAlignment(TextAlign::CENTER);
	label_t1->setSize(vec2(1.0f, 0.1f));
	label_t1->setText("Team Red:");
	label_t1->setFont("Arial");

	team1 = Gui::create<GuiText>(this);
	team1->setPosition(vec2(0.3f, 1.0f));
	team1->setAlignment(TextAlign::CENTER);
	team1->setSize(vec2(1.0f, 0.1f));
	team1->setText("");
	team1->setFont("Arial");

	label_t2 = Gui::create<GuiText>(this);
	label_t2->setPosition(vec2(0.0f, 1.0f));
	label_t2->setAlignment(TextAlign::CENTER);
	label_t2->setSize(vec2(1.0f, 0.1f));
	label_t2->setText("Team Blue:");
	label_t2->setFont("Arial");

	team2 = Gui::create<GuiText>(this);
	team2->setPosition(vec2(0.0f, 1.0f));
	team2->setAlignment(TextAlign::CENTER);
	team2->setSize(vec2(1.0f, 0.1f));
	team2->setText("");
	team2->setFont("Arial");

	auto onClick = std::bind(&GuiTeamMenu::handleSwitch, this);

	swtch = Gui::create<GuiButton>(this);
	swtch->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	swtch->setPosition(vec2(0.2f, 0.3f));
	swtch->setFont("Arial");
	swtch->setSize(vec2(0.6f, 0.25f));
	swtch->setText("switch");
	swtch->addCallback(onClick);

	Network::on(NetMessage::TEAM, boost::bind(&GuiTeamMenu::updateTeamGui, this, _1, _2));
	Network::on(NetMessage::READY, boost::bind(&GuiTeamMenu::setReady, this, _1, _2));
}

void GuiTeamMenu::setPlayerId(int id) {
	this->playerId = id;
}

void GuiTeamMenu::updateTeamGui(Connection *c, NetBuffer &buffer) {

	int size = buffer.read<int>();
	tuple<int, int> temp;
	int t;
	int p;

	for (int i = 0; i < size; i++) {
		temp = buffer.read<tuple<int, int>>();
		player_team[std::get<0>(temp)] = std::get<1>(temp);
	}
	
	std::string team[2];
	
	for (auto it = player_team.begin(); it != player_team.end(); it++) {
		p = it->first;
		t = it->second;
		if (!team[t].empty()) {
			team[t] += ", ";
			team[t] += std::to_string(p);
		}
		else {
			team[t] += std::to_string(p);
		}
	}

	team1->setText(team[0]);
	team2->setText(team[1]);
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
	}
	else {
		message->setText("The game needs 2 players on each team!");
	}
}

bool GuiTeamMenu::getSelectionComplete() {
	return selectionComplete;
}