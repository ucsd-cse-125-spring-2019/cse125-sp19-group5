#pragma once

#include <Shared/Common.h>
#include <vector>

enum GuiState {
	HOVERED,
};

class GuiElement {
	public:
	using Children = std::vector<GuiElement*>;

	bool dispatchMouseButton(float x, float y, int button, int action);
	bool dispatchMousePos(float x, float y);
	bool dispatchKey(const std::string &c);

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
	virtual void onKeyPressed(const std::string &key) { };

	~GuiElement();

	protected:
	GuiElement *parent = nullptr;
	vec2 position;
	vec2 size;
	Children children;
	bool isMouseHovering = false;
};