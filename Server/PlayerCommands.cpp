#include <Shared/Player.h>
#include "GameEngine.h"

void Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			if (std::get<0>(getCooldown(SWING)) > 0) { break; }
			useCooldown(SWING);

			vec3 paddlePosition = getPosition() + getDirection() * vec3(2.05f * getBoundingSphere()->getRadius());
			vec3 paddleVelocity = vec3(getDirection().x, 0, getDirection().z) * vec3((float)(actionCharge));
			int paddleLifespan = 10;

			auto p = gGameEngine->addGameObject<Paddle>();
			p->setPosition(paddlePosition);
			p->setVelocity(paddleVelocity * 0.2f);
			p->setLifespan(paddleLifespan);

			break;
		}
		case SHOOT: {
			if (std::get<0>(getCooldown(SHOOT)) > 0) { break; }
			useCooldown(SHOOT);

			vec3 bulletStart = vec3(getPosition().x, 5, getPosition().z);
			vec3 bulletVelocity = glm::normalize(vec3(getDirection().x, 0, getDirection().z)) * 5.0f;

			auto b = gGameEngine->addGameObject<Bullet>();
			b->setPosition(bulletStart);
			b->setVelocity(bulletVelocity);

			break;
		}
		case WALL: {
			auto wall = gGameEngine->addGameObject<Wall>();
			wall->setPosition(getPosition() + getDirection()*2.0f);
			
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
