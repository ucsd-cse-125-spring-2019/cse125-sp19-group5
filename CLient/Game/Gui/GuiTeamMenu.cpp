#include "GuiTeamMenu.h"

static GuiTeamMenu *teamMenu = nullptr;
static bool firstTime = true;

GuiTeamMenu::GuiTeamMenu()
{
	teamMenu = this;
	setImage("Textures/image2.png");
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	Input::setMouseVisible(true);

	auto mainContainer = Gui::create<GuiRect>(this);
	mainContainer->setPosition(vec2(0.1f, 0.1f));
	mainContainer->setColor(vec4(0.75f, 0.5f, 0.25, 0.0f));
	mainContainer->setSize(vec2(0.8f, 0.8f));

	label = Gui::create<GuiText>(mainContainer);
	label->setPosition(vec2(0.4f, 0.75f));
	label->setAlignment(TextAlign::CENTER);
	label->setText("Team Selection");
	label->setFont("BearPong");
	label->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));

	message = Gui::create<GuiText>(mainContainer);
	message->setPosition(vec2(0.4f, 0.05f));
	message->setAlignment(TextAlign::CENTER);
	message->setText("");
	message->setFont("NormalText");
	message->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));

	GuiText * title_t1 = Gui::create<GuiText>(mainContainer);
	title_t1->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
	title_t1->setPosition(vec2(0.225f, 0.6f));
	title_t1->setFont("NormalText");
	title_t1->setAlignment(TextAlign::CENTER);
	title_t1->setText("red");

	container_t1 = Gui::create<GuiRect>(mainContainer);
	container_t1->setColor(vec4(0.9f, 0.1f, 0.1f, 1.0f) * 0.5f);
	container_t1->setPosition(vec2(0.1f, 0.25f));
	container_t1->setSize(vec2(0.25f / 0.8f, 0.3f / 0.8f));

	GuiText * title_t2 = Gui::create<GuiText>(mainContainer);
	title_t2->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
	title_t2->setPosition(vec2(0.575f, 0.6f));
	title_t2->setFont("NormalText");
	title_t2->setAlignment(TextAlign::CENTER);
	title_t2->setText("blue");

	container_t2 = Gui::create<GuiRect>(mainContainer);
	container_t2->setColor(vec4(0.1f, 0.1f, 0.9f, 1.0f) * 0.5f);
	container_t2->setPosition(vec2(0.45f, 0.25f));
	container_t2->setSize(vec2(0.25f / 0.8f, 0.3f / 0.8f));

	/* Only switch button
	swtch = Gui::create<GuiButton>(mainContainer);
	swtch->setBgColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
	swtch->setPosition(vec2(0.25f, 0.1f));
	swtch->setFont("BearPongSmall");
	swtch->setSize(vec2(0.375f, 0.1f));
	swtch->setText("switch teams");
	swtch->setAlignment(TextAlign::CENTER);
	swtch->setColor(vec4(43.0f / 255.0f, 27.0f / 255.0f, 42.0f / 255.0f, 1.0f));
	auto onClick = std::bind(&GuiTeamMenu::handleSwitch, this);
	swtch->addCallback(onClick);
	*/

	swtch = Gui::create<GuiButton>(mainContainer);
	swtch->setBgColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
	swtch->setPosition(vec2(0.1f, 0.1f));
	swtch->setFont("BearPongSmall");
	swtch->setSize(vec2(0.25f / 0.8f, 0.1f / 0.8f));
	swtch->setText("switch teams");
	swtch->setAlignment(TextAlign::CENTER);
	swtch->setColor(vec4(43.0f / 255.0f, 27.0f / 255.0f, 42.0f / 255.0f, 1.0f));
	auto onClickSwtch = std::bind(&GuiTeamMenu::handleSwitch, this);
	swtch->addCallback(onClickSwtch);

	ready = Gui::create<GuiButton>(mainContainer);
	ready->setBgColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
	ready->setPosition(vec2(0.45f, 0.1f));
	ready->setFont("BearPongSmall");
	ready->setSize(vec2(0.25f / 0.8f, 0.1f / 0.8f));
	ready->setText("ready here");
	ready->setAlignment(TextAlign::CENTER);
	ready->setColor(vec4(43.0f / 255.0f, 27.0f / 255.0f, 42.0f / 255.0f, 1.0f));
	auto onClickRdy = std::bind(&GuiTeamMenu::handleReady, this);
	ready->addCallback(onClickRdy);

	if (firstTime) {
		firstTime = false;
		Network::on(
			NetMessage::TEAM,
			[&](Connection *c, NetBuffer &buffer) {
			if (!teamMenu) { return; }
			teamMenu->updateTeamGui(c, buffer);
		}
		);
		Network::on(
			NetMessage::READY,
			[&](Connection *c, NetBuffer &buffer) {
			if (!teamMenu) { return; }
			teamMenu->setReady(c, buffer);
		}
		);
		Network::on(
			NetMessage::START,
			[&](Connection *c, NetBuffer &buffer) {
			if (!teamMenu) { return; }
			teamMenu->startGame(c, buffer);
		}
		);
	}
}

void GuiTeamMenu::setPlayerId(int id) {
	this->playerId = id;
	label->setText("Team Selection: Player " + std::to_string(playerId));
}

void GuiTeamMenu::updateTeamGui(Connection *c, NetBuffer &buffer) {
	int size = buffer.read<size_t>();
	int t;
	int p;
	float t1_pos = 0.125f;
	float t2_pos = 0.125f;

	id_name = game->getIdName();
	for (size_t i = 0; i < size; i++) {
		auto id = buffer.read<int>();
		auto team = buffer.read<int>();
		player_team[id] = team;
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
			label->setPosition(vec2(-0.03f, t1_pos));
			label->setAlignment(TextAlign::CENTER);
			label->setSize(vec2(1.0f, 0.1f));
			label->setText(id_name[p]);
			label->setFont("NormalText");
			t1_pos += 0.1f;
			team1.push_back(label);
		}
		else {
			label = Gui::create<GuiText>(container_t2);
			label->setPosition(vec2(-0.03f, t2_pos));
			label->setAlignment(TextAlign::CENTER);
			label->setSize(vec2(1.0f, 0.1f));
			label->setText(id_name[p]);
			label->setFont("NormalText");
			t2_pos += 0.1f;
			team2.push_back(label);
		}
	}
}

void GuiTeamMenu::handleSwitch() {
	player_team[playerId] ^= 1;

	NetBuffer playerTeamSelection(NetMessage::TEAM);
	playerTeamSelection.write<int>(player_team.at(playerId));
	Network::send(playerTeamSelection);
}

void GuiTeamMenu::handleReady() {

	NetBuffer readybuf(NetMessage::READY);
	if (isReady) {
		ready->setBgColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
		ready->setColor(vec4(43.0f / 255.0f, 27.0f / 255.0f, 42.0f / 255.0f, 1.0f));
		ready->setText("ready here");
		readybuf.write<bool>(!isReady);
	}
	else {
		ready->setBgColor(vec4(43.0f / 255.0f, 27.0f / 255.0f, 42.0f / 255.0f, 1.0f));
		ready->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
		ready->setText("you are ready");
		readybuf.write<bool>(!isReady);
	}

	Network::send(readybuf);
	isReady = !isReady;
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
	game->setPlayerTeam(player_team);
	remove();
	teamMenu = nullptr;
}

bool GuiTeamMenu::getSelectionComplete() {
	return selectionComplete;
}

void GuiTeamMenu::setGame(Game *game) {
	this->game = game;
}

void GuiTeamMenu::setPlayerTeam(unordered_map<int, int> &p_t) {
	player_team = p_t;
}