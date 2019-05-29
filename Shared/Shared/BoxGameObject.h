#pragma once
#include "GameObject.h"
#include "BoundingBox.h"

class BoxGameObject : public GameObject {
public:
	using GameObject::GameObject;

	BoxGameObject(vec3 position, vec3 direction, int id, float width, float height, float length);
	void setBoundingShape(BoundingBox * box);
	BoundingBox * getBoundingBox();

private:
	vec3 direction;
	float length;
	float width;
	float height;
	BoundingBox * boundingBox = nullptr;
};