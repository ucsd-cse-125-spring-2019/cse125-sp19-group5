#include "GameEngine.h"

void GameEngine::updateGameState(vector<PlayerInputs> playerInputs) {
	vector<int> aggregatePlayerMovements;
	// Use bitwise or to get all player inputs within one server tick
	for (int i = 0; i < NUM_PLAYERS; i++) {
		aggregatePlayerMovements.push_back(0);
	}

	for (PlayerInputs playerInput : playerInputs) {
		aggregatePlayerMovements[playerInput.id] = aggregatePlayerMovements[playerInput.id] | playerInput.inputs;
	}

	for (int i = 0; i < NUM_PLAYERS; i++) {
		aggregatePlayerMovements[i] = aggregatePlayerMovements[i] & MOVEMENT_MASK;
	}

	// Move all players
	for (int i = 0; i < NUM_PLAYERS; i++) {
		// TODO: prevent two players from moving to the same spot
		gameState.players[i]->move(movementInputToVector(gameState.players[i], aggregatePlayerMovements[i]));
	}

	// Process player commands

	// Move all balls
	for (Ball *ball : gameState.balls) {
		ball->move(ball->getVelocity());
	}

	// check collisions
	for (GameObject * gameObject1 : gameState.gameObjects) {
		for (GameObject * gameObject2 : gameState.gameObjects) {
			if (gameObject1->collidesWith(*gameObject2)) {
				gameObject1->onCollision(*gameObject2);
			}
		}
	}
}

GameState & GameEngine::getGameState() {
	return gameState;
}

void GameEngine::addGameObject(Player *player) {
	gameState.gameObjects.push_back(player);
	gameState.players.push_back(player);
}

void GameEngine::addGameObject(Ball *ball) {
	gameState.gameObjects.push_back(ball);
	gameState.balls.push_back(ball);
}

void GameEngine::addGameObject(Wall *wall) {
	gameState.gameObjects.push_back(wall);
	gameState.walls.push_back(wall);
}

vec3 GameEngine::movementInputToVector(Player *player, int movementInput) {
	vec3 direction = glm::normalize(vec3(player->getDirection().x, 0, player->getDirection().z));
	vec3 movement = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
	
	if (movementInput & FORWARD) {
		movement = movement + direction;
	}
	if (movementInput & BACKWARD) {
		movement = movement - direction;
	}
	if (movementInput & LEFT) {
		movement = movement + glm::cross(up, direction);
	}
	if (movementInput & RIGHT) {
		movement = movement - glm::cross(up, direction);
	}

	return glm::normalize(movement); // * player->getSpeed();
	// TODO: implement bhopping
}