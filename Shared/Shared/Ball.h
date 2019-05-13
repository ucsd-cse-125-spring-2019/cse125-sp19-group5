#pragma once
#include "GameObject.h"

class Ball : public GameObject {
public:
	using GameObject::GameObject;

	Ball(vec3 position, vec3 velocity, int id, float radius);
	GAMEOBJECT_TYPES getGameObjectType() const;
	void onCollision(GameObject * gameObject);
	void updateOnServerTick();
};