#pragma once
#include "GameObject.h"

class Wall : public GameObject {
public:
	using GameObject::GameObject;

	Wall(vec3 position, vec3 velocity, int id, int radius);
	GAMEOBJECT_TYPES getGameObjectType() const;
};