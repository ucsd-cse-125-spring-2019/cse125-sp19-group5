#include "GuiConnectMenu.h"
#include "../Networking/Client.h"
#include <Shared/ConfigSettings.h>

GuiConnectMenu::GuiConnectMenu() {
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
	label->setText("Connect to:");
	label->setFont("Arial");

	auto onEnter = std::bind(&GuiConnectMenu::onIpEntered, this, _1);

	ipInput = Gui::create<GuiTextbox>(this);
	ipInput->setBgColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	ipInput->setPosition(vec2(0.2f, 0.3f));
	ipInput->setFont("Arial");
	ipInput->setSize(vec2(0.6f, 0.25f));
	ipInput->addEnterCallback(onEnter);
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