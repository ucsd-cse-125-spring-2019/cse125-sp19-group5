#include "MenuPrompt.h"

MenuPrompt::MenuPrompt()
{
	using std::placeholders::_1;

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
	label->setText("Team Selection: Enter number between 1-4");
	label->setFont("Arial");

	label_t1 = Gui::create<GuiText>(this);
	label_t1->setPosition(vec2(0.0f, 1.0f));
	label_t1->setAlignment(TextAlign::CENTER);
	label_t1->setSize(vec2(1.0f, 0.1f));
	label_t1->setText("Team Blue:");
	label_t1->setFont("Arial");

	team1 = Gui::create<GuiText>(this);
	team1->setPosition(vec2(0.0f, 1.0f));
	team1->setAlignment(TextAlign::CENTER);
	team1->setSize(vec2(1.0f, 0.1f));
	team1->setText("");
	team1->setFont("Arial");

	label_t2 = Gui::create<GuiText>(this);
	label_t2->setPosition(vec2(0.0f, 1.0f));
	label_t2->setAlignment(TextAlign::CENTER);
	label_t2->setSize(vec2(1.0f, 0.1f));
	label_t2->setText("Team Red:");
	label_t2->setFont("Arial");

	team2 = Gui::create<GuiText>(this);
	team2->setPosition(vec2(0.0f, 1.0f));
	team2->setAlignment(TextAlign::CENTER);
	team2->setSize(vec2(1.0f, 0.1f));
	team2->setText("");
	team2->setFont("Arial");

	auto onEnter = std::bind(&MenuPrompt::handleMenuInput, this, _1);

	ipInput = Gui::create<GuiTextbox>(this);
	ipInput->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	ipInput->setPosition(vec2(0.2f, 0.3f));
	ipInput->setFont("Arial");
	ipInput->setSize(vec2(0.6f, 0.25f));
	ipInput->addEnterCallback(onEnter);
}

void MenuPrompt::setGame(Game *game) {
	this->game = game;
	updateTeamGui();
}

void MenuPrompt::setPlayerId(int id) {
	this->playerId = id;
}

void MenuPrompt::updateTeamGui() {
	MenuOptions curr = this->game->getCurrentMenuOptions();
	std::string t1 = "";
	std::string t2 = "";
	if (curr.team_A_1 != -1) {
		t1 = std::to_string(curr.team_A_1);
	}
	if (curr.team_A_2 != -1) {
		if (!t1.empty()) {
			t1 += ", ";
			t1 += std::to_string(curr.team_A_2);
		}
		else {
			t1 = std::to_string(curr.team_A_2);
		}
	}
	if (curr.team_B_1 != -1) {
			t2 = std::to_string(curr.team_A_2);
	}
	if (curr.team_B_2 != -1) {
		if (!t2.empty()) {
			t2 += ", ";
			t2 += std::to_string(curr.team_A_2);
		}
		else {
			t2 = std::to_string(curr.team_A_2);
		}
	}

	team1->setText(t1);
	team2->setText(t2);
}

void MenuPrompt::settingsPrompt() {
	
	/*
	 * The following code draws the "Connect to: " screen
	 */

	 //the whole MenuPrompt is a rectanle, so we make it 
	 //cover the whole the screen
	setColor(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	//Text that says "Select Team: "
	auto label = Gui::create<GuiText>(this);
	label->setPosition(vec2(0.0f, 0.7f));
	label->setAlignment(TextAlign::CENTER);
	label->setSize(vec2(1.0f, 0.1f));
	label->setText("Select Team:");
	label->setFont("Arial");

	//callback - when the user hits the enter key
	auto onEnter = std::bind(&MenuPrompt::handleMenuInput, this, std::placeholders::_1);

	//the input textbox where the user will type in the IP
	auto ipInput = Gui::create<GuiTextbox>(this);
	ipInput->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	ipInput->setPosition(vec2(0.2f, 0.3f));
	ipInput->setFont("Arial");
	ipInput->setSize(vec2(0.6f, 0.25f));
	ipInput->addEnterCallback(onEnter);
	/*
	 * At this point, the "Connect to: " screen has been drawn
	 */
}

void MenuPrompt::handleMenuInput(const std::string &text) {

	int sel = atoi(text.c_str());

	if (sel < 1 || sel > 4) {
		message->setText("Enter number 1, 2, 3, or 4");
		return;
	} 

	NetBuffer playerTeamSelection(NetMessage::TEAM);
	playerTeamSelection.write<int>(sel);
	Network::connection->send(playerTeamSelection);
	Network::on(
		NetMessage::MENU_CONFIRM, 
		boost::bind(&MenuPrompt::handleMenuConfirm, this, _1, _2)
	);
	Network::on(
		NetMessage::MENU_INPUT,
		boost::bind(&MenuPrompt::handlePickAgain, this, _1, _2)
	);
}

void MenuPrompt::handleMenuConfirm(Connection *s, NetBuffer &menuMsg) {
	int confirmedID = menuMsg.read<int>();
	MenuOptions currentMenuOptions = menuMsg.read<MenuOptions>();
	this->game->setCurrentMenuOptions(currentMenuOptions);

	if (confirmedID == playerId) {
		selectionComplete = true;
		remove();
		return;
	}
	else {
		updateTeamGui();
	}
}

void MenuPrompt::handlePickAgain(Connection *s, NetBuffer &menuMsg) {
	MenuOptions currentMenuOptions = menuMsg.read<MenuOptions>();
	this->game->setCurrentMenuOptions(currentMenuOptions);
	message->setText("Invalid choice, please choose again");
	updateTeamGui();
}

bool MenuPrompt::getSelectionComplete() {
	return selectionComplete;
}