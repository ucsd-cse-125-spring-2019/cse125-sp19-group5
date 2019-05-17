#pragma once

#include "GuiText.h"
#include "GuiFocusable.h"

class GuiTextbox : public GuiText, public GuiFocusable {
	public:
	void onKeyPressed(const std::string &key);
};