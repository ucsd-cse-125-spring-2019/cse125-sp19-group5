#pragma once
#include "GameObject.h"

class Paddle : public GameObject {
public:
	Paddle(vec3 position, vec3 velocity, int id, int radius, int lifespan);
	GAMEOBJECT_TYPES getGameObjectType() const;
	bool deleteOnServerTick();
	void updateOnServerTick();

private:
	int lifespan;
};