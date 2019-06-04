#include <Shared/Player.h>
#include "GameEngine.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>

void Player::doAction(PlayerCommands action) {
	switch (action) {
		case SWING: {
			if (std::get<0>(getCooldown(SWING)) > 0) { break; }
			useCooldown(SWING);

			vec3 paddlePosition = getPosition() + getDirection() * vec3(2.05f * getBoundingSphere()->getRadius());
			float y = glm::max(getDirection().y * 0.5f, 0.0f);
			vec3 paddleVelocity = vec3(getDirection().x, y, getDirection().z) * (float)(actionCharge);
			int paddleLifespan = 10;

			auto p = gGameEngine->addGameObject<Paddle>();
			p->setBoundingShape(new BoundingSphere(paddlePosition, 5.0f));
			p->setPosition(paddlePosition);
			p->setVelocity(paddleVelocity * 0.2f);
			p->setLifespan(paddleLifespan);

			break;
		}
		case SHOOT: {
			if (std::get<0>(getCooldown(SHOOT)) > 0) { break; }
			useCooldown(SHOOT);

			float bulletRadius = 0.5f * (actionCharge * 0.2f);
			vec3 bulletStart = getPosition() + (vec3(getDirection().x, 0, getDirection().z) * ((2 * getBoundingSphere()->getRadius()) + bulletRadius));
			vec3 bulletVelocity = glm::normalize(vec3(getDirection().x, 0, getDirection().z)) * 5.0f;

			auto b = gGameEngine->addGameObject<Bullet>();
			b->setBoundingShape(new BoundingSphere(vec3(0.0f), bulletRadius));
			b->setModel("Models/unit_sphere.obj");
			b->setPosition(bulletStart);
			b->setVelocity(bulletVelocity);
			b->setScale(vec3(bulletRadius));

			break;
		}
		case WALL: {
			auto wallPos = getPosition() + getDirection() * 5.0f;
			wallPos.y = 0;
			vec3 size(10.0f, getPosition().y + 3.0f, 2.0f);

			vec3 direction = getDirection();
			direction.y = 0;
			direction = glm::normalize(glm::cross(direction, vec3(0, 1, 0)));

			auto wall = gGameEngine->addGameObject<Wall>();
			wall->setBoundingShape(new BoundingBox(wallPos, direction, size.x, size.y, size.z));
			wall->setPosition(wallPos);
			wall->setModel("Models/unit_cube.obj");
			wall->setMaterial("Materials/brick.json");
			wall->setScale(size);
			
			float yaw = glm::orientedAngle(vec2(direction.x, direction.z), vec2(1, 0));
			quat orientation = glm::eulerAngleY(yaw);
			wall->setOrientation(orientation);

			
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
