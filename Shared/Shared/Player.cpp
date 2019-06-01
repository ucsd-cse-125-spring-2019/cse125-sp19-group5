#include "Player.h"
#include "BoundingSphere.h"
#include <iostream>
#include <glm/gtx/euler_angles.hpp>

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

void Player::updateOnServerTick() {
	for (auto & cd : this->cooldowns) {
		if (std::get<0>(cd.second) > 0) {
			std::get<0>(cd.second) -= 1;
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
	if (glm::length(directionalizedMovement) == 0) { // Avoid returning NaN
		accelDir = vec3(0.0f);
	}
	else {
		accelDir = glm::normalize(directionalizedMovement);
	}
	vec3 newVelocity = getVelocity();
	if (isGrounded) {
		newVelocity.y = PhysicsEngine::applyGravity(vec3(0.0f), PhysicsEngine::getGravity()).y; // Reset gravity
		if (wishJump) {
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
	vec3 newPos = getPosition() + newVelocity * PhysicsEngine::getDeltaTime();
	if (newPos.y < PhysicsEngine::getFloorY()) {
		newPos.y = PhysicsEngine::getFloorY();
		isGrounded = true;
	}

	return newPos;
}

tuple<int, int> & Player::getCooldown(PlayerCommands command) {
	return this->cooldowns[command];
}

void Player::setCooldown(PlayerCommands command, tuple<int, int> cd) {
	this->cooldowns[command] = cd;
}

void Player::useCooldown(PlayerCommands command) {
	tuple<int, int> usedAbility = std::make_tuple(std::get<1>(cooldowns[command]), std::get<1>(cooldowns[command]));
	setCooldown(command, usedAbility);
}

GameObject * Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			if (std::get<0>(getCooldown(SWING)) == 0) {
				useCooldown(SWING);
				vec3 paddlePosition = getPosition() + getDirection() * vec3(2.05f * getBoundingSphere()->getRadius());
				// vec3 paddleVelocity = getDirection() * vec3((float)(actionCharge));
				vec3 paddleVelocity = glm::normalize(vec3(getDirection().x, 0, getDirection().z)) * vec3((float)(actionCharge));
				int paddleLifespan = 10;
				Paddle * p = new Paddle(paddlePosition, paddleVelocity, -1, 5, paddleLifespan);
				/*p->setModel("Models/unit_sphere.obj");
				p->setMaterial("Materials/brick.json");*/
				p->setScale(vec3(5));
				return p;
			}
			return nullptr;
			break;
		}
		case SHOOT: {
			if (std::get<0>(getCooldown(SHOOT)) == 0) {
				useCooldown(SHOOT);
				vec3 bulletStart = vec3(getPosition().x, 5, getPosition().z);
				// vec3 paddleVelocity = getDirection() * vec3((float)(actionCharge));
				vec3 bulletVelocity = glm::normalize(vec3(getDirection().x, 0, getDirection().z)) * 5.0f;
				Bullet * b = new Bullet(bulletStart, bulletVelocity, 1.0f);
				/*b->setModel("Models/unit_sphere.obj");
				b->setMaterial("Materials/brick.json");*/
				return b;
			}
			return nullptr;
			break;
		}
		default: {
			return nullptr;
		}
	}
}

/*
	Takes the byte of player inputs for this player
	Handles only the non-movement commands (charging)
	
	Only one command can charge at one time.
	Once a command has started charging, another cannot start until the action/command has been done.
*/
GameObject * Player::processCommand(int inputs)
{
	vector<PlayerCommands> chargeCommands = { SWING, WALL, SHOOT };
	GameObject * retval = nullptr;

	//TODO for command in chargable commands
	//TODO return GameObject (Ball, Wall) based on input to be rendered by the GameEngine
	// For now, only swing:
	for (auto command : chargeCommands) {
		if (command & inputs) {
			if (!actionCharge) {
				currentAction = command;
				actionCharge = 1;
			}
			else if (command == currentAction) {
				actionCharge++;
			}
		}
		else {
			if (actionCharge && command == currentAction) {
				retval = doAction(command);
				actionCharge = 0;
			}
		}
	}
	return retval;
}

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

void Player::onCollision(Ball * ball) { }

void Player::onCollision(Paddle * paddle) { }

void Player::onCollision(Player * player) { }

void Player::onCollision(Wall * wall) { }
