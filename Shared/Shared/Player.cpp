#include "CollisionDetection.h"
#include "Player.h"
#include "BoundingSphere.h"
#include "Ball.h"
#include "Bullet.h"
#include "Goal.h"
#include "Paddle.h"
#include "PowerUpItem.h"
#include "StunBullet.h"
#include "Wall.h"
#include <iostream>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>
#include <algorithm>

Player::Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team) : SphereGameObject(position, velocity, id, radius) {
	this->direction = direction;
	this->actionCharge = 0;
	this->team = team;
}

GAMEOBJECT_TYPES Player::getGameObjectType() const {
	return PLAYER_TYPE;
}

void Player::setDirection(const vec3 &newDirection) {
	if (glm::length(newDirection) == 0.0f) {
		return;
	}
	direction = glm::normalize(newDirection);
	setOrientation(glm::quatLookAt(direction, vec3(0, 1, 0)));
}

void Player::removePowerup(const POWERUP_TYPES &type) {
	auto it = powerups.find(type);
	if (it == powerups.end()) { return; }

	it->second->onDeactivate();
	delete it->second;
	powerups.erase(it);
}

bool Player::hasPowerup(const POWERUP_TYPES &type) const {
	return powerups.find(type) != powerups.end();
}

void Player::updateOnServerTick() {
	for (auto & cd : this->cooldowns) {
		if (std::get<0>(cd.second) > 0) {
			std::get<0>(cd.second) -= 1;
		}
	}

	if (numLandings == 0) {
		maxBoxHeight = 0.0f;
		isGrounded = false;
	}
	if (!isGrounded && numLandings > 0) {
		setPosition(vec3(getPosition().x, maxBoxHeight, getPosition().z));
		isGrounded = true;
		maxBoxHeight = 0.0f;
	}
	numLandings = 0;

	auto it = powerups.begin();
	while (it != powerups.end()) {
		auto powerup = it->second;
		if (powerup->shouldDeactivate()) {
			powerup->onDeactivate();
			delete powerup;
			it = powerups.erase(it);
		} else {
			it++;
		}
	}
}

vec3 Player::getDirection() {
	return glm::normalize(this->direction);
}

vec3 Player::getMoveDestination(vec3 movement) {
	vec3 direction = glm::normalize(vec3(getDirection().x, 0, getDirection().z));
	vec3 directionalizedMovement = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

	if (movement.z > 0) {
		directionalizedMovement = directionalizedMovement - direction;
	}
	if (movement.z < 0) {
		directionalizedMovement = directionalizedMovement + direction;
	}
	if (movement.x < 0) {
		directionalizedMovement = directionalizedMovement + glm::cross(up, direction);
	}
	if (movement.x > 0) {
		directionalizedMovement = directionalizedMovement - glm::cross(up, direction);
	}

	// For checking frame-perfect bhop
	if (!isJumpKeyDown && !isJumpKey && !isJumpKeyUp && movement.y > 0) {
		isJumpKeyDown = true;
		isJumpKey = false;
		isJumpKeyUp = false;
	}
	else if (isJumpKeyDown && movement.y > 0) {
		isJumpKeyDown = false;
		isJumpKey = true;
		isJumpKeyUp = false;
	}
	else if ((isJumpKeyDown || isJumpKey) && movement.y <= 0) {
		isJumpKeyDown = false;
		isJumpKey = false;
		isJumpKeyUp = true;
	}
	else if (isJumpKey && movement.y > 0) {
		isJumpKeyDown = false;
		isJumpKey = true;
		isJumpKeyUp = false;
	}
	else {
		isJumpKeyDown = false;
		isJumpKey = false;
		isJumpKeyUp = false;
	}

	bool wishJump = false;
	if (PhysicsEngine::getAutoBhopEnabled()) {
		wishJump = movement.y > 0;
	}
	else {
		if (isJumpKeyDown && !wishJump)
			wishJump = true;
		if (isJumpKeyUp)
			wishJump = false;
	}

	// Calculate player velocity
	vec3 accelDir;
	vec3 currVelocity = getVelocity();
	if (glm::length(directionalizedMovement) == 0) { // Avoid returning NaN
		accelDir = vec3(0);
	}
	else {
		accelDir = glm::normalize(directionalizedMovement);
	}
	vec3 newVelocity = getVelocity();
	if (isGrounded) {
		newVelocity.y = PhysicsEngine::applyGravity(vec3(0.0f), PhysicsEngine::getGravity()).y; // Reset gravity
		if (wishJump && !hasPowerup(POWERUP_STUN_DEBUFF)) {
			newVelocity = PhysicsEngine::movePlayerOnGround(accelDir, newVelocity, moveSpeed);
			newVelocity = PhysicsEngine::jumpPlayer(newVelocity);
			isGrounded = false;
			wishJump = false;
		}
		else {
			newVelocity = PhysicsEngine::applyFriction(newVelocity, PhysicsEngine::getPlayerMoveFriction());
			newVelocity = PhysicsEngine::movePlayerOnGround(accelDir, newVelocity, moveSpeed);
		}
	}
	else {
		newVelocity = PhysicsEngine::movePlayerInAir(accelDir, newVelocity, moveSpeed);
		newVelocity = PhysicsEngine::applyGravity(newVelocity, PhysicsEngine::getGravity());
	}
	setVelocity(newVelocity);


	// Prevent the player from ever falling through the floor
	// ATTEMPT TO MOVE GROUND CHECK TO WALL COLLISIONS - KEENAN
	vec3 newPos = getPosition() + newVelocity * PhysicsEngine::getDeltaTime() + currVelocity + this->collisionVelocityComponent;
	this->collisionVelocityComponent = vec3(0);
	/*if (newPos.y < PhysicsEngine::getFloorY()) {
		newPos.y = PhysicsEngine::getFloorY();
		isGrounded = true;
	}*/

	return newPos;
}

tuple<int, int> &Player::getCooldown(PlayerCommands command) {
	return this->cooldowns[command];
}

void Player::setCooldown(PlayerCommands command, tuple<int, int> cd) {
	this->cooldowns[command] = cd;
}

void Player::useCooldown(PlayerCommands command) {
	tuple<int, int> usedAbility = std::make_tuple(std::get<1>(cooldowns[command]), std::get<1>(cooldowns[command]));
	setCooldown(command, usedAbility);
}

#ifdef _CLIENT
void Player::doAction(PlayerCommands action) {
	return nullptr;
}

void Player::processCommand(int inputs) {
	return nullptr;
}

void Player::onCollision(PowerUpItem * item) {
	return;
}
#endif

void Player::serialize(NetBuffer &buffer) const {
	GameObject::serialize(buffer);
	buffer.write(direction);
}

void Player::deserialize(NetBuffer &buffer) {
	GameObject::deserialize(buffer);
	direction = buffer.read<vec3>();
}

void Player::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Player::onCollision(Ball * ball) { 
	float currDist = sqrt(pow(distanceFrom(ball), 2) - pow(getPosition().y - ball->getPosition().y, 2));
	float targetDist = sqrt(pow(getBoundingSphere()->getRadius() + ball->getBoundingSphere()->getRadius(), 2) - pow(getPosition().y - ball->getPosition().y, 2));
	vec3 moveDirection = getPosition() - ball->getPosition();
	moveDirection.y = 0;
	moveDirection = glm::normalize(moveDirection);
	this->collisionVelocityComponent += moveDirection * (targetDist - currDist) * std::max(1.0f, glm::length(ball->getVelocity()));
}

void Player::onCollision(Bullet * bullet) {
	this->collisionVelocityComponent += bullet->getVelocity();
}

void Player::onCollision(Goal * goal) {
	for (Plane * p : CollisionDetection::getIntersectingPlanes(getBoundingSphere(), goal->getBoundingBox())) {
		if (p == goal->getBoundingBox()->top) {
			this->numLandings += 1;
			this->maxBoxHeight = std::max(maxBoxHeight,
				goal->getPosition().y + goal->getBoundingBox()->height + getBoundingSphere()->getRadius());
		}
		else {
			vec3 planeNormal = glm::normalize(p->getNormal());
			float angleBetweenVelocity = glm::angle(glm::normalize(getVelocity()), planeNormal);
			float angleBetweenPosition = glm::angle(glm::normalize(getPosition() - getPrevPosition()), planeNormal);
			if ((angleBetweenVelocity > glm::half_pi<float>()) || (angleBetweenPosition > glm::half_pi<float>())) {
				float planeDistance = abs(p->pointDistance(getPosition()) - (1.01f * getBoundingSphere()->getRadius()));
				setPosition(getPosition() + planeNormal * planeDistance);
			}
		}
	}
}

void Player::onCollision(Paddle * paddle) { }

void Player::onCollision(Player * player) { }

void Player::onCollision(Wall * wall) { 	
	for (Plane * p : CollisionDetection::getIntersectingPlanes(getBoundingSphere(), wall->getBoundingBox())) {
		if (p == wall->getBoundingBox()->bottom) {
			continue;
		}
		if (p == wall->getBoundingBox()->top) {
			this->numLandings += 1;
			this->maxBoxHeight = std::max(maxBoxHeight,
				wall->getPosition().y + wall->getBoundingBox()->height + getBoundingSphere()->getRadius());
			setPosition(vec3(
				getPosition().x,
				wall->getPosition().y + wall->getBoundingBox()->height + getBoundingSphere()->getRadius(),
				getPosition().z)
			);
			break;
		}
		else {
			vec3 planeNormal = glm::normalize(p->getNormal());
			float angleBetweenVelocity = glm::angle(glm::normalize(getVelocity()), planeNormal);
			float angleBetweenPosition = glm::angle(glm::normalize(getPosition() - getPrevPosition()), planeNormal);
			if ((angleBetweenVelocity > glm::half_pi<float>()) || (angleBetweenPosition > glm::half_pi<float>())) {
				float planeDistance = abs(p->pointDistance(getPosition()) - (1.01f * getBoundingSphere()->getRadius()));
				setPosition(getPosition() + planeNormal * planeDistance);
			}
		}
	}
}

void Player::setMoveSpeed(float newMoveSpeed) {
	moveSpeed = newMoveSpeed;
}

float Player::getMoveSpeed() const {
	return moveSpeed;
}

void Player::setStrength(float newStrength) {
	strength = newStrength;
}

float Player::getStrength() const {
	return strength;
}

void Player::setNumBullets(int numBullets) {
	this->numBullets = numBullets;
}

int Player::getNumBullets() const {
	return numBullets;
}

void Player::setBulletType(BULLET_TYPES bulletType) {
	this->bulletType = bulletType;
}
BULLET_TYPES Player::getBulletType() const {
	return this->bulletType;
}