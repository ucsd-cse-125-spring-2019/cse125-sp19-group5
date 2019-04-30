#pragma once
#include "GameObject.h"

class Ball : public GameObject {
public:
	Ball(vec3 position, vec3 velocity, string id, int radius);
	void onCollision(GameObject * gameObject);
	void updateOnServerTick();
};