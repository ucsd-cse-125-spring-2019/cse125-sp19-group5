#pragma once

#include "GuiElement.h"

class GuiFocusable {
	private:
	static GuiFocusable *currFocused;

	public:
	bool requestFocus();
	void unfocus();
	bool isFocused() const;

	virtual bool shouldGiveUpFocus(GuiFocusable *requester);
	virtual void onFocusChanged(bool gained);
};