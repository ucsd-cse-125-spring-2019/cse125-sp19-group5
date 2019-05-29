#include <Shared/Player.h>
#include "GameEngine.h"

void Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			// std::cout << "Swing with charge " << actionCharge << std::endl;
			// assumes direction is unit vector
			vec3 paddlePosition = getPosition() + getDirection() * vec3(2.05f * this->radius);
			// vec3 paddleVelocity = getDirection() * vec3((float)(actionCharge));
			vec3 paddleVelocity = vec3(getDirection().x, 0, getDirection().z) * vec3((float)(actionCharge));
			int paddleLifespan = 100;

			auto p = gGameEngine->addGameObject<Paddle>();
			p->setPosition(paddlePosition);
			p->setVelocity(paddleVelocity);
			p->setLifespan(paddleLifespan);

			break;
		}
		case WALL: {
			break;
		}
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
				doAction(command);
				actionCharge = 0;
			}
		}
	}
}
