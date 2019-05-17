#include "GuiTextbox.h"

void GuiTextbox::onKeyPressed(const std::string &key) {
	if (isFocused()) {
		setText(getText() + key);
	}
}