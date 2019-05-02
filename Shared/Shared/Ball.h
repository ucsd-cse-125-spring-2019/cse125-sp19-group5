#pragma once
#include "GameObject.h"

class Ball : public GameObject {
public:
	Ball(vec3 position, vec3 velocity, int id, int radius);
	int getGameObjectType() const;
	void onCollision(GameObject * gameObject);
	void updateOnServerTick();
};