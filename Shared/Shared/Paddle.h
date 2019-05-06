#pragma once
#include "GameObject.h"

class Paddle : public GameObject {
public:
	Paddle(vec3 position, vec3 velocity, int id, float radius, int lifespan);
	int getGameObjectType() const;
	bool deleteOnServerTick();
	void updateOnServerTick();

private:
	int lifespan;
};