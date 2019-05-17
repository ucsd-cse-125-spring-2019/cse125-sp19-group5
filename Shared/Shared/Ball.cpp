#include "Ball.h"
#include "BoundingSphere.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Ball::Ball(vec3 position, vec3 velocity, int id, float radius) : GameObject(position, velocity, id) {
	setBoundingShape(new BoundingSphere(position, radius));
}

GAMEOBJECT_TYPES Ball::getGameObjectType() const {
	return BALL_TYPE;
}

void Ball::updateOnServerTick() {
	if (glm::length(getVelocity()) > 0) { 
		vec3 direction = glm::normalize(getVelocity());
		vec3 updatedVelocity = getVelocity() - (direction * 0.1f);
		vec3 updatedDirection = glm::normalize(updatedVelocity);
		if (glm::length(glm::dot(direction, updatedDirection)) < 0.0f) {
			setVelocity(vec3(0.0f));
		}
		else {
			setVelocity(updatedVelocity);
		}
	}
}


void Ball::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
	/*switch (gameObject->getGameObjectType()) {
	case PADDLE_TYPE:
		std::cout << to_string() << " collided with " << gameObject->to_string() << std::endl;
		std::cout << to_string() << " velocity changing from " << glm::to_string(getVelocity()) << " to " << glm::to_string(gameObject->getVelocity()) << std::endl;
		setVelocity(gameObject->getVelocity());
		break;
	case WALL_TYPE:
		std::cout << to_string() + " collided with " << gameObject->to_string() << std::endl;
		vector<vec3> planeNormals;
		break;
	}*/
}

void Ball::onCollision(Ball * ball) { }

void Ball::onCollision(Paddle * paddle) {
	std::cout << to_string() << " collided with " << paddle->to_string() << std::endl;
	std::cout << to_string() << " velocity changing from " << glm::to_string(getVelocity()) << " to " << glm::to_string(paddle->getVelocity()) << std::endl;
	setVelocity(paddle->getVelocity());
}


void Ball::onCollision(Player * player) { }

void Ball::onCollision(Wall * wall) {
	std::cout << to_string() + " collided with " << wall->to_string() << std::endl;
	vector<vec3> planeNormals;
	setVelocity(getVelocity() * vec3(-1));
}