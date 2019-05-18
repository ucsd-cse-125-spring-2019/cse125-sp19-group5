#pragma once

#include "GuiText.h"
#include "GuiFocusable.h"

class GuiTextbox : public GuiText, public GuiFocusable, public GuiBgColorable {
	protected:
	vec4 cursorColor = vec4(1.0f);
	int cursorIndex = 0;
	float cursorX = 0.0f;

	int getDrawStartIndex();

	public:
	bool onKeyPressed(Gui::Key key) override;
	bool onCharPressed(const std::string &c) override;
	void draw(float x, float y, float w, float h) const override;
	void onMouseButton(float x, float y, int button, int action) override;

	void setCursorIndex(int i = -1);
	void updateCursorPos();
	void drawCursor(float x, float y, float w, float h) const;

	virtual void onTextChanged();
};