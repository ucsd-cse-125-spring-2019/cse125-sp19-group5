#include "Paddle.h"

Paddle::Paddle (vec3 position, vec3 velocity, string id, int radius, int lifespan) : GameObject(position, velocity, id, radius) {
	this->lifespan = lifespan;
}

bool Paddle::deleteOnServerTick() {
	return --lifespan == 0;
}