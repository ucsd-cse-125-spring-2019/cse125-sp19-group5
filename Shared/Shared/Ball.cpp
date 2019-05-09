#include "Ball.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Ball::Ball(vec3 position, vec3 velocity, int id, int radius) : GameObject(position, velocity, id, radius) {
}

GAMEOBJECT_TYPES Ball::getGameObjectType() const {
	return BALL_TYPE;
}

void Ball::onCollision(GameObject * gameObject) {
	switch (gameObject->getGameObjectType()) {
	case PADDLE_TYPE:
		std::cout << to_string() + " collided with " + gameObject->to_string() << std::endl;
		std::cout << to_string() + " velocity changing from " + glm::to_string(getVelocity()) + " to " + glm::to_string(gameObject->getVelocity()) << std::endl;
		setVelocity(gameObject->getVelocity());
		break;
	}
}

void Ball::updateOnServerTick() {
	if (glm::length(getVelocity()) > 0) { 
		vec3 direction = glm::normalize(getVelocity());
		vec3 updatedVelocity = getVelocity() - (direction * 0.1f);
		vec3 updatedDirection = glm::normalize(updatedVelocity);
		if (glm::length(glm::dot(direction, updatedDirection)) < 0) {
			setVelocity(vec3(0.0f));
		}
		else {
			setVelocity(updatedVelocity);
		}
	}
}