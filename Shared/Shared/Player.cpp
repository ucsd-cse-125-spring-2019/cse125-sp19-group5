#include "Player.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Player::Player(vec3 position, vec3 velocity, vec3 direction, string id, int radius) : GameObject(position, velocity, id, radius) {
	this->direction = direction;
	this->actionCharge = 0;
}

void Player::onCollision(GameObject &gameObject) {
	std::cout << "Player: " + id + glm::to_string(position) + " collided with " + gameObject.getId() + glm::to_string(gameObject.getPosition()) << std::endl;
}

vec3 Player::getDirection() {
	return this->direction;
}

void Player::doAction(PlayerCommands action)
{
	switch (action)
	{
	case SWING:
		std::cout << "Swing with charge " << actionCharge << std::endl;
		// TODO: do the swing
		break;
	default:
		break;
	}
}

/*
	Takes the byte of player inputs for this player
	Handles only the non-movement commands (charging)
	
	Only one command can charge at one time.
	Once a command has started charging, another cannot start until the action/command has been done.
*/
void Player::processCommand(int inputs)
{
	vector<PlayerCommands> chargeCommands = { SWING, WALL };

	//TODO for command in chargable commands
	// For now, only swing:
	for (auto command : chargeCommands) {
		if (command & inputs) {
			if (!actionCharge) {
				currentAction = command;
			}
			else if (command == currentAction) {
				actionCharge++;
			}
		}
		else {
			if (actionCharge) {
				doAction(command);
				actionCharge = 0;
			}
		}
	}
}
