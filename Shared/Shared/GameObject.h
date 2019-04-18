#pragma once
#include "Common.h"

class GameObject {
public:
	bool collidesWith(GameObject &gameObject);
	void onCollision(GameObject &gameObject);
	double distanceFrom(GameObject &gameObject);

private:
	vec3 position;
	vec3 velocity;
	string id;
	int radius;
};