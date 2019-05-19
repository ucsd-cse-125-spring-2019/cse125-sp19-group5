#include "Ball.h"
#include "BoundingSphere.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>


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
}

void Ball::onCollision(Ball * ball) { }

void Ball::onCollision(Paddle * paddle) {
	/*std::cout << to_string() << " collided with " << paddle->to_string() << std::endl;
	std::cout << to_string() << " velocity changing from " << glm::to_string(getVelocity()) << " to " << glm::to_string(paddle->getVelocity()) << std::endl;*/
	setVelocity(paddle->getVelocity());
}


void Ball::onCollision(Player * player) { }

void Ball::onCollision(Wall * wall) {
	// TODO: FIX CORNER COLLISIONS, TREAT CORNER AS A SPHERE?
	// TODO: CORRECT PLANE INTERSECTION TEST INSTEAD OF CLOSEST, I.E. CHECK TWO PLANE COLLISION FOR CORNERS
	/*std::cout << to_string() + " collided with " << wall->to_string() << std::endl;*/

	if (getVelocity() == vec3(0.0f)) {
		return;
	}

	// get planes that ball collides with
	vector<Plane * > collisionPlanes;
	for (Plane * p : wall->getBoundingBox()->getFacePlanes()) {
		vec3 planeNormal = glm::normalize(p->getNormal());
		float angleBetween = glm::angle(glm::normalize(getVelocity()), planeNormal);		
		if (angleBetween > glm::half_pi<float>() && angleBetween < (3.0f * glm::half_pi<float>())) {
			collisionPlanes.push_back(p);
		}
	}

	// get closest plane
	Plane * closestPlane = nullptr;
	for (Plane * p : collisionPlanes) {
		if (!closestPlane) {
			closestPlane = p;
		}
		else {
			if (abs(p->pointDistance(getPosition())) < abs(closestPlane->pointDistance(getPosition()))) {
				closestPlane = p;
			}
		}
	}

	setVelocity(glm::reflect(getVelocity(), glm::normalize(closestPlane->getNormal())));
}