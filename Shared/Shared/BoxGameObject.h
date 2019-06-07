#pragma once
#include "GameObject.h"
#include "BoundingBox.h"

class BoxGameObject : public GameObject {
public:
	using GameObject::GameObject;

	BoxGameObject(vec3 position, vec3 direction, int id, float width, float height, float length);
	vec3 setPosition(vec3 position);
	void setBoundingShape(BoundingBox * box);
	BoundingBox * getBoundingBox();

private:
	vec3 direction;
	BoundingBox * boundingBox = nullptr;
};