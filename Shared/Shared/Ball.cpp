#include "Ball.h"
#include "CollisionDetection.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>


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
	this->goalScored = false;
}

bool Ball::getGoalScored() {
	return this->goalScored;
}


void Ball::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Ball::onCollision(Ball * ball) { 
	if (collidesWith(ball)) {
		if (glm::length(getPosition() - ball->getPosition()) == 0.0f) {
			setPosition(getPosition() + vec3((rand() % 100) / 100.0f, 0, (rand() % 100) / 100.0f));
		}

		if ((glm::length(getVelocity()) == 0.0f) && (glm::length(ball->getVelocity()) == 0.0f)) {
			return;
		}

		vec3 newVelocity = getVelocity();
		newVelocity += glm::proj(ball->getVelocity(), ball->getPosition() - getPosition());
		newVelocity -= glm::proj(getVelocity(), getPosition() - ball->getPosition());

		vec3 ballNewVelocity = ball->getVelocity();
		ballNewVelocity += glm::proj(getVelocity(), ball->getPosition() - getPosition());
		ballNewVelocity -= glm::proj(ball->getVelocity(), getPosition() - ball->getPosition());

		setVelocity(newVelocity);
		ball->setVelocity(ballNewVelocity);

		std::cout << glm::length(newVelocity) << " " << glm::length(ballNewVelocity) << std::endl;

		float intersectDist = getBoundingSphere()->getRadius() + ball->getBoundingSphere()->getRadius() - distanceFrom(ball);
		while (collidesWith(ball)) {
			move(glm::normalize(newVelocity) * (intersectDist / 2.0f));
			ball->move(glm::normalize(ballNewVelocity) * (intersectDist / 2.0f));
		}
	}	
}

void Ball::onCollision(Goal * goal) {
	setPosition(vec3(0, 4, 0));
	setVelocity(vec3(0));
	this->goalScored = true;
}

void Ball::onCollision(Paddle * paddle) {
	/*std::cout << to_string() << " collided with " << paddle->to_string() << std::endl;
	std::cout << to_string() << " velocity changing from " << glm::to_string(getVelocity()) << " to " << glm::to_string(paddle->getVelocity()) << std::endl;*/
	setVelocity(paddle->getVelocity());
}


void Ball::onCollision(Player * player) { 
	setVelocity(vec3(0.0f));
}

void Ball::onCollision(Wall * wall) {
	// TODO: FIX CORNER COLLISIONS, TREAT CORNER AS A SPHERE?
	// TODO: account for multiple planes colliding (corners?)

	if (getVelocity() == vec3(0.0f)) {
		return;
	}

	vector<Plane * > collisionPlanes;
	for (Plane * p : CollisionDetection::getIntersectingPlanes(this->getBoundingSphere(), wall->getBoundingBox())) {
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

	// move sphere so it no longer intersects with plane
	if (closestPlane) {
		float planeDistance = abs(closestPlane->pointDistance(getPosition()) - (1.01f * getBoundingSphere()->getRadius()));
		move(glm::normalize(closestPlane->getNormal()) * planeDistance);
		setVelocity(glm::reflect(getVelocity(), glm::normalize(closestPlane->getNormal())));
	}
}