#include "Ball.h"
#include "CollisionDetection.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>
#include "Bullet.h"
#include "Goal.h"
#include "Paddle.h"
#include "Player.h"
#include "Wall.h"
#include "PhysicsEngine.h"


GAMEOBJECT_TYPES Ball::getGameObjectType() const {
	return BALL_TYPE;
}

void Ball::updateOnServerTick() {
	if (glm::length(getVelocity()) > 0) {
		vec3 direction = glm::normalize(getVelocity());
		vec3 updatedVelocity = getVelocity() - (direction * 0.01f);
		// vec3 updatedVelocity = getVelocity() * 0.99f;
		vec3 updatedDirection = glm::normalize(updatedVelocity);
		if (glm::dot(direction, updatedDirection) < 0.0f) {
			setVelocity(vec3(0.0f));
		}
		else {
			setVelocity(updatedVelocity);
		}
	}
	this->goalScored = false;

	for (auto it = currentBallCollisions.begin(); it != currentBallCollisions.end(); ) {
		if (!collidesWith(*it)) {
			it = currentBallCollisions.erase(it);
		}
		else {
			++it;
		}
	}

	if (!this->isGrounded) {
		setVelocity(PhysicsEngine::applyGravity(getVelocity(), PhysicsEngine::getGravity() * 5.0f));
	}

	if (ticksSinceGrounded > GROUNDED_TICKS_THRESHOLD) {
		isGrounded = false;
	}

	this->ticksSinceGrounded += 1;

	soundHitTimer = glm::max(0.0f, soundHitTimer - PhysicsEngine::getDeltaTime());
	soundBounceTimer = glm::max(0.0f, soundBounceTimer - PhysicsEngine::getDeltaTime());
	soundOofTimer = glm::max(0.0f, soundOofTimer - PhysicsEngine::getDeltaTime());

	if (glm::all(glm::isnan(getPosition()))) {
		vec3 prev = getPrevPosition();
		setPosition(prev);
		this->prevPosition = prev;
	}
}

bool Ball::getGoalScored() {
	return this->goalScored;
}


void Ball::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Ball::onCollision(Ball * ball) { 
	if (currentBallCollisions.find(ball) == currentBallCollisions.end()) {
		currentBallCollisions.insert(ball);
		ball->currentBallCollisions.insert(this);

		if (collidesWith(ball)) {
			// currentCollisions.insert(ball);
			if (glm::length(getPosition() - ball->getPosition()) == 0.0f) {
				move(vec3((rand() % 100) / 100.0f, 0, (rand() % 100) / 100.0f));
				std::cout << to_string() << std::endl;
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

			float velLength = glm::length(newVelocity) + glm::length(ballNewVelocity);
			newVelocity = glm::normalize(newVelocity) * (velLength / 2.0f);
			ballNewVelocity = glm::normalize(ballNewVelocity) * (velLength / 2.0f);

			setVelocity(newVelocity);
			ball->setVelocity(ballNewVelocity);

			this->lastHitBy = ball->lastHitBy;
		}
	}
}

void Ball::onCollision(Bullet * bullet) {
	// std::cout << bullet->to_string() << std::endl;
	setVelocity(getVelocity() + bullet->getVelocity() * 0.3f);

	if (soundHitTimer <= 0.0f) {
		string soundToPlay = soundHit[getRandIndex(soundHit.size())];
		this->playSound(soundToPlay, 1.0f, false);
		soundHitTimer = SOUND_HIT_CD;
	}
}

void Ball::onCollision(Goal * goal) {
	setPosition(goal->getRandomSpawnPos());
	setVelocity(vec3(0));
	this->isGrounded = false;
	this->goalScored = true;

	if (this->lastHitBy->getTeam() == goal->getTeam()) {
		this->lastHitBy->setGoalsScored(this->lastHitBy->getGoalsScored() - 1);
	}
	else {
		this->lastHitBy->setGoalsScored(this->lastHitBy->getGoalsScored() + 1);
	}
	this->playSound(soundCrowdCheer, 1.0f, false);
}

void Ball::onCollision(Paddle * paddle) {
	/*std::cout << to_string() << " collided with " << paddle->to_string() << std::endl;
	std::cout << to_string() << " velocity changing from " << glm::to_string(getVelocity()) << " to " << glm::to_string(paddle->getVelocity()) << std::endl;*/
	if (paddle->getObjectsHit().find(this) == paddle->getObjectsHit().end()) {
		setVelocity(paddle->getVelocity());
		paddle->getObjectsHit().insert(this);
		currentBallCollisions.clear();
		this->isGrounded = false;

		this->lastHitBy = paddle->getOwner();

		if (soundHitTimer <= 0.0f) {
			string soundToPlay = soundHit[getRandIndex(soundHit.size())];
			this->playSound(soundToPlay, 1.0f, false);
			soundHitTimer = SOUND_HIT_CD;
		}
	}
}


void Ball::onCollision(Player * player) { 
	// setVelocity(getVelocity() * 0.9f);

	if (soundOofTimer <= 0.0f) {
		if (glm::length(getVelocity()) > 4.0f) {
			this->playSound(soundPlayerSuperOof, 1.0f, false);
			soundOofTimer = SOUND_OOF_CD;
		}
		else if (glm::length(getVelocity()) > 2.0f) {
			this->playSound(soundPlayerOof, 1.0f, false);
			soundOofTimer = SOUND_OOF_CD;
		}
	}
}

void Ball::onCollision(Wall * wall) {
	// TODO: FIX CORNER COLLISIONS, TREAT CORNER AS A SPHERE?
	// TODO: account for multiple planes colliding (corners?)

	if (getVelocity() == vec3(0.0f)) {
		return;
	}

	bool didCollideSide = false;

	vector<Plane * > collisionPlanes;
	for (Plane * p : CollisionDetection::getIntersectingPlanes(this->getBoundingSphere(), wall->getBoundingBox())) {
		if (p == wall->getBoundingBox()->top) {
			if (!this->isGrounded) {
				if (this->ticksSinceGrounded < GROUNDED_TICKS_THRESHOLD) {
					isGrounded = true;
					this->velocity.y = 0;
					vec3 adjustedPos = getPosition();
					adjustedPos.y = wall->getPosition().y + wall->getBoundingBox()->height + getBoundingSphere()->getRadius();
					setPosition(adjustedPos);
				}
			}
			this->ticksSinceGrounded = 0;
		}
		else {
			didCollideSide = true;
		}
		vec3 planeNormal = glm::normalize(p->getNormal());
		float angleBetween = glm::angle(glm::normalize(getVelocity()), planeNormal);
		if (angleBetween > glm::half_pi<float>()) {
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
		float planeDistance = abs(closestPlane->pointDistance(getPosition()) - (getBoundingSphere()->getRadius()));
		move(glm::normalize(closestPlane->getNormal()) * planeDistance);
		setVelocity(glm::reflect(getVelocity(), glm::normalize(closestPlane->getNormal())));
	}

	if (soundBounceTimer <= 0.0f && (glm::length(getVelocity().y) > 0.19f || didCollideSide)) {
		string soundToPlay = soundBounce[getRandIndex(soundBounce.size())];
		this->playSound(soundToPlay, 1.0f, false);
		soundBounceTimer = SOUND_BOUNCE_CD;
	}
}