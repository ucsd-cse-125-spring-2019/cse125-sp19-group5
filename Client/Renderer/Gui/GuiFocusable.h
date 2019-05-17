#pragma once

#include "GuiElement.h"

class GuiFocusable : public GuiElement {
	private:
	static GuiFocusable *currFocused;

	public:
	bool requestFocus();
	void unfocus();
	bool isFocused() const;
	virtual void onMouseButton(float x, float y, int button, int action);

	virtual bool shouldGiveUpFocus(GuiFocusable *requester);
	virtual void onFocusChanged(bool gained);
};