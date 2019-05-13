#pragma once
#include "GameObject.h"

struct Plane;

class Wall : public GameObject {
public:
	using GameObject::GameObject;

	Wall(vec3 position, vec3 direction, int id, float length, float width, float height);
	GAMEOBJECT_TYPES getGameObjectType() const;

private:
	vec3 direction;
	float length;
	float width;
	float height;
};