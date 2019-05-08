#include "GameEngine.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Networking/Server.h"

template<class T, class V>
void inline safeRemoveFromVec(std::vector<T> &v, V &val) {
	auto it = std::find(v.begin(), v.end(), val);
	if (it != v.end()) {
		v.erase(it);
	}
}

void GameEngine::removeGameObjectById(int id) {
	auto gameObject = gameState.gameObjects[id];
	if (gameObject) {
		gameState.gameObjects[id] = nullptr;
		safeRemoveFromVec(gameState.players, gameObject);
		safeRemoveFromVec(gameState.walls, gameObject);
		safeRemoveFromVec(gameState.balls, gameObject);
		delete gameObject;
	}
}

void GameEngine::onPlayerDisconnected(Connection *c) {
	removeGameObjectById(c->getId());
}

void GameEngine::updateGameState(vector<PlayerInputs> & playerInputs) {
	
	movePlayers(playerInputs);
	doPlayerCommands(playerInputs);

	moveBalls();

	doCollisionInteractions();
	updateGameObjectsOnServerTick();
	removeDeadObjects();
	// send getNetworkGameState() to client
}

GameState & GameEngine::getGameState() {
	return gameState;
}

void GameEngine::addGenericGameObject(GameObject *obj) {
	gameState.gameObjects[obj->getId()] = obj;

	NetBuffer buffer(NetMessage::GAME_OBJ_CREATE);
	buffer.write<GAMEOBJECT_TYPES>(obj->getGameObjectType());
	obj->serialize(buffer);
	Network::broadcast(buffer);
}

void GameEngine::addGameObject(Player *player) {
	addGenericGameObject(player);
	gameState.players.push_back(player);
}

void GameEngine::addGameObject(Ball *ball) {
	addGenericGameObject(ball);
	gameState.balls.push_back(ball);
}

void GameEngine::addGameObject(Wall *wall) {
	addGenericGameObject(wall);
	gameState.walls.push_back(wall);
}

vec3 GameEngine::movementInputToVector(int movementInput) {
	vec3 movement = vec3(0.0f);

	if ((movementInput & MOVEMENT_MASK) == 0) {
		return movement;
	}
	
	if (movementInput & FORWARD) {
		movement = movement + vec3(0, 0, -1);
	}
	if (movementInput & BACKWARD) {
		movement = movement + vec3(0, 0, 1);
	}
	if (movementInput & LEFT) {
		movement = movement + vec3(-1, 0, 0);
	}
	if (movementInput & RIGHT) {
		movement = movement + vec3(1, 0, 0);
	}

	if (glm::length(movement) == 0.0f) {
		return vec3(0.0f);
	}

	return glm::normalize(movement);
}

void GameEngine::movePlayers(vector<PlayerInputs> & playerInputs) {
	vector<int> aggregatePlayerMovements;
	//cout << "playerInputs size: " << playerInputs.size() << endl;
	// Use bitwise or to get all player inputs within one server tick
	for (int i = 0; i < gameState.players.size(); i++) {
		aggregatePlayerMovements.push_back(0);
	}
	for (PlayerInputs playerInput : playerInputs) {
		aggregatePlayerMovements[playerInput.id] = aggregatePlayerMovements[playerInput.id] | playerInput.inputs;
	}
	for (int i = 0; i < gameState.players.size(); i++) {
		aggregatePlayerMovements[i] = aggregatePlayerMovements[i] & MOVEMENT_MASK;
}
	// Move all players
	for (int i = 0; i < gameState.players.size(); i++) {
		// TODO: prevent two players from moving to the same spot
		if (gameState.players[i] == nullptr) {
			std::cerr << "Whoa" << std::endl;
		}
		// gameState.players[i]->move(movementInputToVector(aggregatePlayerMovements[i]));
		noCollisionMove(gameState.players[i], movementInputToVector(aggregatePlayerMovements[i]));
		//cout << aggregatePlayerMovements[i] << "   " << glm::to_string(gameState.players[i]->getPosition()) << endl;
	}
}

void GameEngine::moveBalls() {
	for (Ball *ball : gameState.balls) {
		noCollisionMove(ball, ball->getVelocity());
	}
}

void GameEngine::doPlayerCommands(vector<PlayerInputs> & playerInputs) {
	vector<int> aggregatePlayerCommands;

	// Use bitwise or to get all player inputs within one server tick
	for (int i = 0; i < gameState.players.size(); i++) {
		aggregatePlayerCommands.push_back(0);
	}
	for (PlayerInputs playerInput : playerInputs) {
		aggregatePlayerCommands[playerInput.id] = aggregatePlayerCommands[playerInput.id] | playerInput.inputs;
	}
	for (int i = 0; i < gameState.players.size(); i++) {
		aggregatePlayerCommands[i] = aggregatePlayerCommands[i] & COMMAND_MASK;
	}

	// Process player commands
	for (int i = 0; i < gameState.players.size(); i++) {
		GameObject * createdGameObject = gameState.players[i]->processCommand(aggregatePlayerCommands[i]);
		if (createdGameObject) {
			gameState.gameObjects.push_back(createdGameObject);
		}
	}
}

void GameEngine::doCollisionInteractions() {
	for (GameObject * gameObject1 : gameState.gameObjects) {
		if (!gameObject1) { continue; }
		for (GameObject * gameObject2 : gameState.gameObjects) {
			if (gameObject2 && gameObject1->collidesWith(gameObject2)) {
				gameObject1->onCollision(gameObject2);
			}
		}
	}
}

void GameEngine::removeDeadObjects() {
	vector<GameObject *> preservedGameObjects;
	for (GameObject * gameObject : gameState.gameObjects) {
		if (gameObject && gameObject->deleteOnServerTick()) {
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
		if (gameObject) {
			gameObject->updateOnServerTick();
		}
	}
}

GameStateNet * GameEngine::getGameStateNet(GameStateNet * networkGameState) {
	// not sure if this method should return a pointer or not?
	// potential issue of returning reference to local variable
	// if not a reference does send(getNetworkGameState()) create a duplicate?
	// if it is a problem should we use pointers instead?
	//GameStateNet * networkGameState = new GameStateNet();

	networkGameState->in_progress = gameState.in_progress;
	networkGameState->score = gameState.score;
	networkGameState->timeLeft = gameState.timeLeft;
	networkGameState->gameObjects.clear();

	for (GameObject * gameObject : gameState.gameObjects) {
		if (gameObject) {
			//cout << "COPYING GAME OBJECT!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			networkGameState->gameObjects.push_back(gameObject);
		}
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
	//cout << "destination=" << glm::to_string(destination) << "  movement=" << glm::to_string(movement) << endl;

	return true;
}

const std::vector<GameObject*> &GameEngine::getGameObjects() const {
	return gameState.gameObjects;
}
