#pragma once

#include "GuiText.h"
#include "GuiFocusable.h"
#include <functional>

class GuiTextbox : public GuiText, public GuiFocusable, public GuiBgColorable {
	public:
	using EnterCallback = std::function<void(const std::string&)>;

	protected:
	std::vector<EnterCallback> enterCallbacks;
	std::string drawText = "";
	vec4 cursorColor = vec4(1.0f);
	int cursorIndex = 0;
	float cursorX = 0.0f;

	void backspace();
	int getDrawStartIndex();

	public:

	bool dispatchKey(Gui::Key key, Gui::KeyState state) override;
	bool onCharPressed(const std::string &c) override;
	void draw(float x, float y, float w, float h) const override;
	void onMouseButton(float x, float y, int button, int action) override;

	void setCursorIndex(int i = -1);
	void updateCursorPos();
	void drawCursor(float x, float y, float w, float h) const;

	void setText(const std::string &newText);

	virtual void onTextChanged();

	void addEnterCallback(const EnterCallback &callback);
	virtual void onEnter(std::string text);
};