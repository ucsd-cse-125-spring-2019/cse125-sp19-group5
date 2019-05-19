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

void GuiTextbox::backspace() {
	if (text.empty()) { return; }
	cursorIndex--;
	text.erase(text.begin() + cursorIndex);
	updateCursorPos();
	onTextChanged();
}

void GuiTextbox::onTextChanged() {
}

void GuiTextbox::draw(float x, float y, float w, float h) const {
	Draw::setColor(bgColor);
	Draw::rect(x, y, w, h);

	gTextRenderer->renderText(
		drawText,
		font,
		x, y,
		color,
		scale
	);

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
	size_t drawStart = 0;
	size_t drawEnd = 0;

	auto maxWidth = getScreenSize().x;

	auto subStr = text.substr(0, cursorIndex);
	auto subStrWidth = gTextRenderer->getTextSize(subStr, font, scale).x;
	cursorX = subStrWidth / Draw::screenWidth;

	auto clampedCursorX = glm::clamp(cursorX, 0.0f, maxWidth);
	auto spaceRight = cursorX + maxWidth - clampedCursorX;
	auto spaceLeft = cursorX - clampedCursorX;
	auto curX = 0.0f;

	for (size_t i = 0; i < text.size(); i++) {
		auto c = text.substr(i, 1);
		auto widthPixels = gTextRenderer->getTextSize(c, font, scale).x;
		auto width = widthPixels / Draw::screenWidth;
		if (curX < spaceLeft) {
			drawStart++;
		} else if (curX <= spaceRight) {
			drawEnd++;
		}
		curX += width;
	}

	drawText = text.substr(drawStart, drawEnd);
	cursorX = clampedCursorX;
}

int GuiTextbox::getDrawStartIndex() {
	return 0;
}

bool GuiTextbox::dispatchKey(Gui::Key key, Gui::KeyState state) {
	// Only capture key inputs when focused.
	if (!isFocused()) { return GuiText::dispatchKey(key, state);  }

	// Ignore key released.
	if (state == Gui::KeyState::RELEASED) {
		return true;
	}

	switch (key) {
		// Delete current character when pressing backspace.
		case Gui::Key::BACKSPACE:
			backspace();
			break;
		// Move cursor with left, right arrows.
		case Gui::Key::LEFT:
			if (cursorIndex > 0) {
				setCursorIndex(cursorIndex - 1);
			}
			break;
		case Gui::Key::RIGHT:
			if (cursorIndex < text.size()) {
				setCursorIndex(cursorIndex + 1);
			}
			break;
		// Move to end when pressing end key.
		case Gui::Key::END:
			setCursorIndex();
			break;
	}
	return true;
}

void GuiTextbox::drawCursor(float x, float y, float w, float h) const {
	if (!isFocused()) { return; }

	auto border = CURSOR_BORDER / Draw::screenHeight;
	auto width = CURSOR_WIDTH / Draw::screenWidth;

	Draw::setColor(cursorColor);
	Draw::rect(x + cursorX, y + border, width, h - (2.0f * border));
}