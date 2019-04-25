#include "Ball.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Ball::Ball(vec3 position, vec3 velocity, string id, int radius) : GameObject(position, velocity, id, radius) {
}

void Ball::onCollision(GameObject * gameObject) {
	if (gameObject->getGameObjectType() == "paddle") {
		std::cout << getId() + " collided with " + gameObject->getId() << std::endl;
		std::cout << getId() + " velocity changing from " + glm::to_string(getVelocity()) + " to " + glm::to_string(gameObject->getVelocity()) << std::endl;
		setVelocity(gameObject->getVelocity());
	}
}

void Ball::updateOnServerTick() {
	std::cout << getId() << glm::to_string(getPosition()) << std::endl;
}