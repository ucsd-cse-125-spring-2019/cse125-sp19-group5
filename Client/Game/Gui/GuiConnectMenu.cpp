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
		100, "Fonts/Arial.ttf"
	});

	Input::setMouseVisible(true);

	GuiText *title = Gui::create<GuiText>(this);
	title->setPosition(vec2(0.0f, 0.7f));
	title->setAlignment(TextAlign::CENTER);
	title->setSize(vec2(1.0f, 0.2f));
	title->setText("Bear Pong");
	title->setFont("BearPong");

	message = Gui::create<GuiText>(this);
	message->setPosition(vec2(0.0f, 0.7f));
	message->setColor(vec4(1.0f, 0.1f, 0.1, 1.0f));
	message->setText("");
	message->setSize(vec2(1.0f, 0.1f));
	message->setFont("Arial");
	message->setAlignment(TextAlign::CENTER);

	label = Gui::create<GuiText>(this);
	label->setPosition(vec2(0.0f, 0.4f));
	label->setAlignment(TextAlign::CENTER);
	label->setSize(vec2(1.0f, 0.1f));
	label->setText("Connect to:");
	label->setFont("Arial");

	//auto onEnter = std::bind(&GuiConnectMenu::onIpEntered, this, _1);

	ipInput = Gui::create<GuiTextbox>(this);
	ipInput->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	ipInput->setPosition(vec2(0.2f, 0.3f));
	ipInput->setFont("Arial");
	ipInput->setSize(vec2(0.6f, 0.12f));
	//ipInput->addEnterCallback(onEnter);

	GuiText *nameLabel = Gui::create<GuiText>(this);
	nameLabel->setPosition(vec2(0.0f, 0.6f));
	nameLabel->setAlignment(TextAlign::CENTER);
	nameLabel->setSize(vec2(1.0f, 0.1f));
	nameLabel->setText("Your Name:");
	nameLabel->setFont("Arial");

	nameInput = Gui::create<GuiTextbox>(this);
	nameInput->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	nameInput->setPosition(vec2(0.2f, 0.5f));
	nameInput->setFont("Arial");
	nameInput->setSize(vec2(0.6f, 0.12f));

	enter = Gui::create<GuiButton>(this);
	enter->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	enter->setPosition(vec2(0.35f, 0.1f));
	enter->setFont("Arial");
	enter->setSize(vec2(0.3f, 0.1f));
	enter->setText("Enter");
	enter->setAlignment(TextAlign::CENTER);

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
		remove();
	}
	else {
		message->setText("Failed to connect!");
	}
}