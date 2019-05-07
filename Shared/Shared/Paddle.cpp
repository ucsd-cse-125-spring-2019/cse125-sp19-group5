#include "Paddle.h"

Paddle::Paddle (vec3 position, vec3 velocity, int id, int radius, int lifespan) : GameObject(position, velocity, id, radius) {
	this->lifespan = lifespan;
}

GAMEOBJECT_TYPES Paddle::getGameObjectType() const {
	return PADDLE_TYPE;
}

bool Paddle::deleteOnServerTick() {
	return lifespan == 0;
}

void Paddle::updateOnServerTick() {
	--lifespan;
}