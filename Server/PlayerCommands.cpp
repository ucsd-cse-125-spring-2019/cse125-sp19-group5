#include <Shared/Player.h>
#include "GameEngine.h"

void Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			if (std::get<0>(getCooldown(SWING)) > 0) { break; }
			useCooldown(SWING);

			vec3 paddlePosition = getPosition() + getDirection() * vec3(2.05f * getBoundingSphere()->getRadius());
			float y = glm::max(getDirection().y * 0.5f, 0.0f);
			vec3 paddleVelocity = vec3(getDirection().x, y, getDirection().z) * vec3((float)(actionCharge));
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
			b->setBoundingShape(new BoundingSphere(vec3(0.0f), 0.5f));
			b->setModel("Models/unit_sphere.obj");
			b->setPosition(bulletStart);
			b->setVelocity(bulletVelocity);
			b->setScale(vec3(0.5f));

			break;
		}
		case WALL: {
			auto wallPos = getPosition() + getDirection() * 5.0f;
			wallPos.y = 0;
			vec3 size(10.0f, 5.0f, 2.0f);
			auto wall = gGameEngine->addGameObject<Wall>();
			wall->setBoundingShape(new BoundingBox(wallPos, vec3(1, 0, 0), size.x, size.y, size.z));
			wall->setPosition(wallPos);
			wall->setModel("Models/unit_cube.obj");
			wall->setMaterial("Materials/brick.json");
			wall->setScale(size);
			
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
	vector<PlayerCommands> chargeCommands = { SWING, WALL, SHOOT };

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
