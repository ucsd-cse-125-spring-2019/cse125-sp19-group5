#include "GuiFocusable.h"

GuiFocusable *GuiFocusable::currFocused = nullptr;

bool GuiFocusable::requestFocus() {
	if (currFocused) {
		if (!currFocused->shouldGiveUpFocus(this)) {
			return false;
		}
		onFocusChanged(false);
	}
	currFocused = this;
	onFocusChanged(true);
	return true;
}

void GuiFocusable::unfocus() {
	currFocused = nullptr;
	onFocusChanged(false);
}

bool GuiFocusable::isFocused() const {
	return this == currFocused;
}

bool GuiFocusable::shouldGiveUpFocus(GuiFocusable *requester) {
	return true;
}

void GuiFocusable::onFocusChanged(bool gained) { }