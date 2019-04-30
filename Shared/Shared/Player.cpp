#include "Player.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Player::Player(vec3 position, vec3 velocity, vec3 direction, string id, int radius) : GameObject(position, velocity, id, radius) {
	this->direction = direction;
	this->actionCharge = 0;
}

void Player::onCollision(GameObject * gameObject) {
	std::cout << "Player: " + id + glm::to_string(position) + " collided with " + gameObject->getId() + glm::to_string(gameObject->getPosition()) << std::endl;
}

vec3 Player::getDirection() {
	return this->direction;
}

GameObject * Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			// std::cout << "Swing with charge " << actionCharge << std::endl;
			// assumes direction is unit vector
			vec3 paddlePosition = getPosition() + getDirection()*vec3(2 * radius);
			vec3 paddleVelocity = getDirection() * vec3(actionCharge);
			string paddleId = "paddle_" + std::to_string(getIntId());
			int paddleLifespan = 10;
			return new Paddle(paddlePosition, paddleVelocity, paddleId, 1, paddleLifespan);
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
