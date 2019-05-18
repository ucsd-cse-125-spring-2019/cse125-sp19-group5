#include "GuiTextbox.h"
#include "../Draw.h"
#include "../TextRenderer.h"

constexpr auto CURSOR_BORDER = 2.0f; // pixels
constexpr auto CURSOR_WIDTH = 1.0f; // pixels

bool GuiTextbox::onCharPressed(const std::string &c) {
	if (!isFocused()) { return false; }
	text.insert(text.begin() + cursorIndex, c[0]);
	setCursorIndex(cursorIndex + 1);
	onTextChanged();
	return false;
}

bool GuiTextbox::onKeyPressed(Gui::Key key) {
	if (!isFocused()) { return false; }

	switch (key) {
		case Gui::Key::BACKSPACE:
			if (!text.empty()) {
				text.pop_back();
				setCursorIndex(cursorIndex - 1);
				onTextChanged();
			}
			break;
	}

	return false;
}

void GuiTextbox::onTextChanged() {
}

void GuiTextbox::draw(float x, float y, float w, float h) const {
	Draw::setColor(bgColor);
	Draw::rect(x, y, w, h);
	GuiText::draw(x, y, w, h);

	drawCursor(x, y, w, h);
}

void GuiTextbox::onMouseButton(float x, float y, int button, int action) {
	if (isMouseHovering) {
		requestFocus();
	}
}

void GuiTextbox::setCursorIndex(int i) {
	if (i < 0) {
		i = static_cast<int>(text.size());
	}
	cursorIndex = i;
	updateCursorPos();
}

void GuiTextbox::updateCursorPos() {
	auto subStr = text.substr(0, cursorIndex);
	auto subStrWidth = gTextRenderer->getTextSize(subStr, font, scale).x;
	cursorX = subStrWidth / Draw::screenWidth;
}

int GuiTextbox::getDrawStartIndex() {
	return 0;
}

void GuiTextbox::drawCursor(float x, float y, float w, float h) const {
	if (!isFocused()) { return; }

	auto border = CURSOR_BORDER / Draw::screenHeight;
	auto width = CURSOR_WIDTH / Draw::screenWidth;

	Draw::setColor(cursorColor);
	Draw::rect(x + cursorX, y + border, width, h - (2.0f * border));
}