#include "MenuPrompt.h"

MenuPrompt::MenuPrompt()
{
	ipPrompt
}

bool MenuPrompt::ipPrompt() {
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
	auto onEnter = std::bind(&MenuPrompt::onIpEntered, this, _1);

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

void MenuPrompt::settingPrompt() {

}

void MenuPrompt::onIpEntered(const std::string &text) {
	
	Network::Init(text, 1234);
	
}