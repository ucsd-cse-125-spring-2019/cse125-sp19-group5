#include "GameEngine.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

void GameEngine::updateGameState(vector<PlayerInputs> playerInputs) {
	movePlayers(playerInputs);
	doPlayerCommands(playerInputs);

	// Move all balls
	// this could potentially be performed by a gameObject's updateOnServerTick since it should
	// be the same on every server tick -- ball's movement is only dependent on its velocity
	for (Ball *ball : gameState.balls) {
		noCollisionMove(ball, ball->getVelocity());
	}

	doCollisionInteractions();
	updateGameObjectsOnServerTick();
	removeDeadObjects();
	// send getNetworkGameState() to client
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

vec3 GameEngine::movementInputToVector(int movementInput) {
	vec3 movement = vec3(0.0f, 0.0f, 0.0f);

	if ((movementInput & MOVEMENT_MASK) == 0) {
		return movement;
	}
	
	if (movementInput & FORWARD) {
		movement = movement + vec3(0, 0, 1);
	}
	if (movementInput & BACKWARD) {
		movement = movement - vec3(0, 0, -1);
	}
	if (movementInput & LEFT) {
		movement = movement + vec3(-1, 0, 0);
	}
	if (movementInput & RIGHT) {
		movement = movement + vec3(1, 0, 0);
	}

	return glm::normalize(movement);
}

void GameEngine::movePlayers(vector<PlayerInputs> playerInputs) {
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
		// gameState.players[i]->move(movementInputToVector(aggregatePlayerMovements[i]));
		noCollisionMove(gameState.players[i], movementInputToVector(aggregatePlayerMovements[i]));
	}
}

void GameEngine::doPlayerCommands(vector<PlayerInputs> playerInputs) {
	vector<int> aggregatePlayerCommands;

	// Use bitwise or to get all player inputs within one server tick
	for (int i = 0; i < NUM_PLAYERS; i++) {
		aggregatePlayerCommands.push_back(0);
	}
	for (PlayerInputs playerInput : playerInputs) {
		aggregatePlayerCommands[playerInput.id] = aggregatePlayerCommands[playerInput.id] | playerInput.inputs;
	}
	for (int i = 0; i < NUM_PLAYERS; i++) {
		aggregatePlayerCommands[i] = aggregatePlayerCommands[i] & COMMAND_MASK;
	}

	// Process player commands
	for (int i = 0; i < NUM_PLAYERS; i++) {
		GameObject * createdGameObject = gameState.players[i]->processCommand(aggregatePlayerCommands[i]);
		if (createdGameObject) {
			gameState.gameObjects.push_back(createdGameObject);
		}
	}
}

void GameEngine::doCollisionInteractions() {
	for (GameObject * gameObject1 : gameState.gameObjects) {
		for (GameObject * gameObject2 : gameState.gameObjects) {
			if (gameObject1->collidesWith(gameObject2)) {
				gameObject1->onCollision(gameObject2);
			}
		}
	}
}

void GameEngine::removeDeadObjects() {
	vector<GameObject *> preservedGameObjects;
	for (GameObject * gameObject : gameState.gameObjects) {
		if (gameObject->deleteOnServerTick()) {
			delete gameObject;
		}
		else {
			preservedGameObjects.push_back(gameObject);
		}
	}
	gameState.gameObjects = preservedGameObjects;
}

void GameEngine::updateGameObjectsOnServerTick() {
	for (GameObject * gameObject : gameState.gameObjects) {
		gameObject->updateOnServerTick();
	}
}

GameStateNet & GameEngine::getGameStateNet() {
	// not sure if this method should return a pointer or not?
	// potential issue of returning reference to local variable
	// if not a reference does send(getNetworkGameState()) create a duplicate?
	// if it is a problem should we use pointers instead?
	GameStateNet networkGameState;

	networkGameState.in_progress = gameState.in_progress;
	networkGameState.score = gameState.score;
	networkGameState.timeLeft = gameState.timeLeft;

	for (GameObject * gameObject : gameState.gameObjects) {
		networkGameState.gameObjects.push_back(*gameObject);
	}

	return networkGameState;
}

bool GameEngine::noCollisionMove(GameObject * gameObject, vec3 movement) {
	vec3 destination = gameObject->getMoveDestination(movement);

	for (GameObject * otherGameObject : gameState.gameObjects) {
		if (gameObject != gameObject) {
			float distance = glm::distance(destination, otherGameObject->getPosition());
			if (distance < (gameObject->getRadius() + otherGameObject->getRadius())) {
				return false;
			}
		}
	}

	gameObject->setPosition(destination);
	return true;
}