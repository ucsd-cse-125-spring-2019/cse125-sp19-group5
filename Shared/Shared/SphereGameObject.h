#pragma once
#include "GameObject.h"
#include "BoundingSphere.h"

class SphereGameObject : public GameObject {
public:
	using GameObject::GameObject;

	SphereGameObject(vec3 position, vec3 velocity, int id, float radius);
	void setBoundingShape(BoundingSphere * sphere);
	BoundingSphere * getBoundingSphere();

private:
	BoundingSphere * boundingSphere = nullptr;
};