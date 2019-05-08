#include "Player.h"
#include "BoundingSphere.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Player::Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team) : GameObject(position, velocity, id) {
	this->direction = direction;
	this->radius = radius;
	this->actionCharge = 0;
	this->team = team;
	setBoundingShape(new BoundingSphere(position, radius));
}

int Player::getGameObjectType() const {
	return PLAYER_TYPE;
}

void Player::onCollision(GameObject * gameObject) {
	std::cout << to_string() + " collided with " << gameObject->to_string() << std::endl;
}

vec3 Player::getDirection() {
	return this->direction;
}

vec3 Player::getMoveDestination(vec3 movement) {
	if (movement.x == 0.0f && movement.z == 0.0f) {
		return getPosition();
	}

	vec3 direction = glm::normalize(vec3(getDirection().x, 0, getDirection().z));
	vec3 directionalizedMovement = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

	if (movement.z > 0) {
		directionalizedMovement = directionalizedMovement + direction;
	}
	if (movement.z < 0) {
		directionalizedMovement = directionalizedMovement - direction;
	}
	if (movement.x < 0) {
		directionalizedMovement = directionalizedMovement + glm::cross(up, direction);
	}
	if (movement.x > 0) {
		directionalizedMovement = directionalizedMovement - glm::cross(up, direction);
	}

	return getPosition() + glm::normalize(directionalizedMovement); // * player->getSpeed();
	// TODO: implement bhopping
}

GameObject * Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			// std::cout << "Swing with charge " << actionCharge << std::endl;
			// assumes direction is unit vector
			vec3 paddlePosition = getPosition() + getDirection() * vec3(2.05f * this->radius);
			vec3 paddleVelocity = getDirection() * vec3((float)(actionCharge));
			int paddleLifespan = 10;
			return new Paddle(paddlePosition, paddleVelocity, getId(), 1, paddleLifespan);
			break;
		}
		default: {
			return NULL;
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
	vector<PlayerCommands> chargeCommands = { SWING, WALL };
	GameObject * retval = NULL;

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
