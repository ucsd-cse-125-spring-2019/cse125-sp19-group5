#include "Paddle.h"
#include "BoundingSphere.h"

Paddle::Paddle (vec3 position, vec3 velocity, int id, float radius, int lifespan) : GameObject(position, velocity, id) {
	this->lifespan = lifespan;
	setBoundingShape(new BoundingSphere(position, radius));
}

int Paddle::getGameObjectType() const {
	return PADDLE_TYPE;
}

bool Paddle::deleteOnServerTick() {
	return lifespan == 0;
}

void Paddle::updateOnServerTick() {
	--lifespan;
}