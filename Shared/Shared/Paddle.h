#pragma once
#include "GameObject.h"

class Paddle : public GameObject {
public:
	Paddle(vec3 position, vec3 velocity, string id, int radius, int lifespan);
	bool deleteOnServerTick();
	void updateOnServerTick();

private:
	int lifespan;
};