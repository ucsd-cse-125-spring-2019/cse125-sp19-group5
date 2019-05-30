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
	label->setText("Team Selection:");
	label->setFont("Arial");

	label_t1 = Gui::create<GuiText>(this);
	label_t1->setPosition(vec2(0.0f, 1.0f));
	label_t1->setAlignment(TextAlign::CENTER);
	label_t1->setSize(vec2(1.0f, 0.1f));
	label_t1->setText("Team Blue:");
	label_t1->setFont("Arial");

	auto onEnter = std::bind(&MenuPrompt::handleMenuOptions, this, _1);

	ipInput = Gui::create<GuiTextbox>(this);
	ipInput->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	ipInput->setPosition(vec2(0.2f, 0.3f));
	ipInput->setFont("Arial");
	ipInput->setSize(vec2(0.6f, 0.25f));
	ipInput->addEnterCallback(onEnter);
}

void MenuPrompt::setGame(Game *game) {
	this->game = game;
}

void MenuPrompt::ipPrompt() {
	/*
	 * The following code draws the "Connect to: " screen
	 */

	 //the whole MenuPrompt is a rectanle, so we make it 
	 //cover the whole the screen
	setColor(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	//Text that says "Connect to: "
	auto label = Gui::create<GuiText>(this);
	label->setPosition(vec2(0.0f, 0.7f));
	label->setAlignment(TextAlign::CENTER);
	label->setSize(vec2(1.0f, 0.1f));
	label->setText("Connect to:");
	label->setFont("Arial");

	//callback - when the user hits the enter key
	auto onEnter = std::bind(&MenuPrompt::onIpEntered, this, std::placeholders::_1);

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

void MenuPrompt::onIpEntered(const std::string &text) {
	connected = Network::init(text, 1234);
}

bool MenuPrompt::getConnected() {
	return connected;
}

void MenuPrompt::handleMenuInput(const std::string &text) {
	Network::on(NetMessage::MENU_CONFIRM, [this](Connection *s, NetBuffer &buffer) {

	});
}