#pragma once

#include "GuiKey.h"
#include <Shared/Common.h>
#include <vector>

enum GuiState {
	HOVERED,
};

class GuiBgColorable {
	protected:
	vec4 bgColor = vec4(1.0f);

	public:
	void setBgColor(const vec4 &newBgColor) { bgColor = newBgColor; }
	const vec4 &getBgColor() const { return bgColor; }
};

class GuiElement {
	public:
	using Children = std::vector<GuiElement*>;

	bool dispatchMouseButton(float x, float y, int button, int action);
	bool dispatchMousePos(float x, float y);
	bool dispatchKey(Gui::Key key, bool pressed);
	bool dispatchChar(const std::string &c);

	void addChild(GuiElement *newChild);
	void removeChild(GuiElement *child);
	void clearChildren();
	GuiElement *getParent() const;
	Children &getChildren();

	// Position = position relative to bottom left corner of parent.
	void setPosition(const vec2 &newPosition);
	const vec2 &getPosition() const;

	// Returns the absolute position bottom left corner on the screen.
	vec2 getScreenPos(const vec2 &pos = vec2(0.0f)) const;

	void setSize(const vec2 &newSize);
	const vec2 &getSize() const;

	// Returns the size of the element relative to the screen.
	vec2 getScreenSize() const;

	virtual void update(float dt);

	virtual void draw(float x, float y, float w, float h) const;
	void drawElement(const vec2 &offset = vec2(0.0f)) const;

	void remove();

	bool isAbsPosWithin(float posX, float posY) const;

	// Mouse events.
	virtual void onMouseEntered(float x, float y) { };
	virtual void onMouseLeft(float x, float y) { };
	virtual void onMouseButton(float x, float y, int button, int action) { };

	// Called when a character has been pressed. Return `true` to block
	// other downstream elements from receiving the character.
	virtual bool onCharPressed(const std::string &c) { return false; };

	// Called when a key (a non-printable character) has been pressed.
	// Return `true` to block downstream elements from receiving this.
	virtual bool onKeyPressed(Gui::Key key) { return false; };

	virtual ~GuiElement();

	protected:
	GuiElement *parent = nullptr;
	vec2 position;
	vec2 size;
	Children children;
	bool isMouseHovering = false;
};