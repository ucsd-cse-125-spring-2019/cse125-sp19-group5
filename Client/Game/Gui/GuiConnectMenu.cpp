#include "GuiConnectMenu.h"
#include "../Networking/Client.h"
#include <Shared/ConfigSettings.h>

GuiConnectMenu::GuiConnectMenu() {
	using std::placeholders::_1;

	//setColor(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	setImage("Textures/image1.png");
	setPosition(vec2(0.0f, 0.0f));
	setSize(vec2(1.0f, 1.0f));

	gTextRenderer->loadFont("BearPong", {
		100, "Fonts/PermanentMarker.ttf"
	});

	gTextRenderer->loadFont("BearPongSmall", {
		50, "Fonts/PermanentMarker.ttf"
		});

	gTextRenderer->loadFont("NormalText", {
		40, "Fonts/HammersmithOne.ttf"
	});

	Input::setMouseVisible(true);

	GuiText *title = Gui::create<GuiText>(this);
	title->setPosition(vec2(0.0f, 0.7f));
	title->setAlignment(TextAlign::CENTER);
	title->setSize(vec2(1.0f, 0.2f));
	title->setText("Bear Pong");
	title->setFont("BearPong");
	title->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));

	message = Gui::create<GuiText>(this);
	message->setPosition(vec2(0.0f, 0.7f));
	message->setColor(vec4(1.0f, 0.1f, 0.1, 1.0f));
	message->setText("");
	message->setSize(vec2(1.0f, 0.1f));
	message->setFont("NormalText");
	message->setAlignment(TextAlign::CENTER);

	label = Gui::create<GuiText>(this);
	label->setPosition(vec2(0.0f, 0.4f));
	label->setAlignment(TextAlign::CENTER);
	label->setSize(vec2(1.0f, 0.1f));
	label->setText("connect ip");
	label->setFont("NormalText");
	label->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));

	//auto onEnter = std::bind(&GuiConnectMenu::onIpEntered, this, _1);

	GuiRect *ipInputBox = Gui::create<GuiRect>(this);
	ipInputBox->setColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	ipInputBox->setPosition(vec2(0.2f, 0.3f));
	ipInputBox->setSize(vec2(0.6f, 0.11f));

	ipInput = Gui::create<GuiTextbox>(ipInputBox);
	ipInput->setBgColor(vec4(0.0f));
	ipInput->setPosition(vec2(0.01f, 0.02f));
	ipInput->setFont("NormalText");
	ipInput->setSize(vec2((0.6f - 0.02f)/0.6f, (0.11f - 0.04f)/0.11f));
	//ipInput->addEnterCallback(onEnter);

	GuiText *nameLabel = Gui::create<GuiText>(this);
	nameLabel->setPosition(vec2(0.0f, 0.6f));
	nameLabel->setAlignment(TextAlign::CENTER);
	nameLabel->setSize(vec2(1.0f, 0.1f));
	nameLabel->setText("name");
	nameLabel->setFont("NormalText");
	nameLabel->setColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));

	GuiRect *nameInputBox = Gui::create<GuiRect>(this);
	nameInputBox->setColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	nameInputBox->setPosition(vec2(0.2f, 0.5f));
	nameInputBox->setSize(vec2(0.6f, 0.11f));

	nameInput = Gui::create<GuiTextbox>(nameInputBox);
	nameInput->setBgColor(vec4(0.0f));
	nameInput->setPosition(vec2(0.01f, 0.02f));
	nameInput->setFont("NormalText");
	nameInput->setSize(vec2((0.6f - 0.02f) / 0.6f, (0.11f - 0.04f) / 0.11f));

	enter = Gui::create<GuiButton>(this);
	enter->setBgColor(vec4(167.0f / 255.0f, 136.0f / 255.0f, 99.0f / 255.0f, 1.0f));
	enter->setPosition(vec2(0.4f, 0.15f));
	enter->setFont("BearPongSmall");
	enter->setSize(vec2(0.2f, 0.1f));
	enter->setText("join lobby");
	enter->setAlignment(TextAlign::CENTER);
	enter->setColor(vec4(43.0f / 255.0f, 27.0f / 255.0f, 42.0f / 255.0f, 1.0f));

	auto onClick = std::bind(&GuiConnectMenu::handleEnter, this);
	enter->addCallback(onClick);
}

void GuiConnectMenu::onIpEntered(const std::string &text) {
	auto port = 1234;
	ConfigSettings::get().getValue("Port", port);
	if (Network::init(text, port)) {
		remove();
	} else {
		message->setText("Failed to connect!");
	}
}

void GuiConnectMenu::handleEnter() {
	auto port = 1234;
	ConfigSettings::get().getValue("Port", port);
	std::string ip = ipInput->getText();
	std::string name = nameInput->getText();
	if (Network::init(ip, port)) {
		NetBuffer namebuf(NetMessage::NAME);
		namebuf.write<std::string>(name);
		Network::send(namebuf);
		game->teamMenu();
		remove();
	}
	else {
		message->setText("Failed to connect!");
	}
}

void GuiConnectMenu::setGame(Game * game) {
	this->game = game;
}