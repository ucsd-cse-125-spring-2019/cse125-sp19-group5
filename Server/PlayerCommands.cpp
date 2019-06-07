#include <Shared/Bomb.h>
#include <Shared/Paddle.h>
#include <Shared/Player.h>
#include <Shared/StunBullet.h>
#include "GameEngine.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
			p->setVelocity(paddleVelocity * 0.2f * strength);
			p->setLifespan(paddleLifespan);
			p->setOwner(this);
			p->setModel("Models/static_paddle.obj");
			p->setScale(vec3(1.5f));
			if (gGameEngine->player_team[getId()] == 0) {
				p->setMaterial("Materials/paddle_red.json");
			}
			else {
				p->setMaterial("Materials/paddle_blue.json");
			}

			vec3 direction = getDirection();
			direction.y = 0;
			direction = glm::normalize(glm::cross(direction, vec3(0, 1, 0)));
			float yaw = glm::orientedAngle(vec2(direction.x, direction.z), vec2(1, 0));
			quat orientation = glm::eulerAngleY(yaw);
			p->setOrientation(orientation);


			break;
		}
		case SHOOT: {
			if (std::get<0>(getCooldown(SHOOT)) > 0) { break; }
			useCooldown(SHOOT);
			
			currentlySwinging = true;
			setAnimation(2);
			gGameEngine->setTimer(std::to_string(getId()) + "_swing", 0.1f,
				[&]() {
					currentlySwinging = false;
				}
			);

			if (hasPowerup(POWERUP_BOMBS)) {
				float bombRadius = 1.0f;
				vec3 bombVelocity = getDirection() * (float)(actionCharge) * 0.3f * strength;
				vec3 bombStart = getPosition() + getDirection() * (1.1f * getBoundingSphere()->getRadius() + bombRadius);
				
				Bomb * bomb = gGameEngine->addGameObject<Bomb>();
				bomb->setBoundingShape(new BoundingSphere(bombStart, bombRadius));
				bomb->setPosition(bombStart);
				bomb->setVelocity(bombVelocity);
				bomb->setModel("Models/bomb.obj");
				bomb->setMaterial("Materials/bomb.json");
				bomb->setScale(vec3(bombRadius));
				bomb->setOwner(this);
			}
			else {
				float bulletRadius = 0.5f * (actionCharge * 0.2f);
				for (int i = 0; i < numBullets; i++) {
					float angle = (i - numBullets / 2) * 5;
					vec3 bulletDir = glm::normalize(vec3(getDirection().x, 0, getDirection().z));
					bulletDir = glm::normalize(glm::rotateY(bulletDir, glm::radians(angle)));
					vec3 bulletStart = getPosition() + (bulletDir * (1.1f * getBoundingSphere()->getRadius() + bulletRadius));
					vec3 bulletVelocity = glm::normalize(bulletDir) * 3.0f;

					Bullet * b = nullptr;
					switch (getBulletType()) {
					case BULLET_STUN: {
						b = gGameEngine->addGameObject<StunBullet>();
						b->setModel("Models/beehive.obj");
						b->setMaterial("Materials/beehive_yellow.json");
						break;
					}
					default: {
						b = gGameEngine->addGameObject<Bullet>();
						b->setModel("Models/unit_sphere.obj");
						b->setMaterial("Materials/brick.json");
						break;
					}
					}
					b->setBoundingShape(new BoundingSphere(vec3(0.0f), bulletRadius));
					b->setPosition(bulletStart);
					b->setVelocity(bulletVelocity);
					b->setScale(vec3(bulletRadius));
					b->setOwner(this);
				}
			}

			break;
		}
		case WALL: {
			if (std::get<0>(getCooldown(WALL)) > 0) { break; }
			useCooldown(WALL);
			auto wallPos = getPosition() + getDirection() * 5.0f;
			wallPos.y = -(getPosition().y + 3.0f);
			vec3 size(10.0f, getPosition().y + 3.0f, 2.0f);

			vec3 direction = getDirection();
			direction.y = 0;
			direction = glm::normalize(glm::cross(direction, vec3(0, 1, 0)));

			auto wall = gGameEngine->addGameObject<Wall>();
			wall->setBoundingShape(new BoundingBox(wallPos, direction, size.x, size.y, size.z));
			wall->setDestroyable(true);
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
				if (actionCharge >= 20) {
					actionCharge = 20;
				}
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

void Player::updateAnimations() {
	if (!currentlySwinging) {
		if (this->isGrounded) {
			if (glm::length(getVelocity()) == 0) {
				setAnimation(3, false);
			}
			else {
				setAnimation(0, false);
			}
		}
		else {
			setAnimation(1, false);
		}
	}
}