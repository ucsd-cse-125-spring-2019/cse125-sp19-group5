#pragma once

#include "../Renderer/Gui/Gui.h"
#include "../Renderer/Gui/GuiTextbox.h"
#include "../Renderer/Gui/GuiButton.h"
#include "../Renderer/Gui/GuiRect.h"
#include "../Renderer/Gui/GuiText.h"
#include "../../Shared/Shared/CommonStructs.h"
#include "../../Networking/Client.h"

class MenuPrompt: public GuiRect {
	private:
		int playerId


		void onIpEntered(const std::string &text);
		void onPortEntered(const std::string &text);

		void handleMenuOptions();
		void handleMenuConfirmed();
		
		bool ipPrompt();
		void settingPrompt();

	public:
		MenuPrompt();
		~MenuPrompt();
};
