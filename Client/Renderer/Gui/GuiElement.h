#pragma once

#include <Shared/Common.h>
#include <vector>

class GuiElement {
	public:
	using Children = std::vector<GuiElement*>;

	void addChild(GuiElement *newChild);
	void removeChild(GuiElement *child);
	void clearChildren();
	GuiElement *getParent() const;
	Children &getChildren();

	void setPosition(const vec2 &newPosition);
	const vec2 &getPosition() const;

	void setSize(const vec2 &newSize);
	const vec2 &getSize() const;

	virtual void update(float dt);

	virtual void draw(float x, float y, float w, float h) const;
	void drawElement(const vec2 &offset = vec2(0.0f)) const;

	~GuiElement();

	protected:
	GuiElement *parent = nullptr;
	vec2 position;
	vec2 size;
	Children children;
};