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

		NetBuffer buffer(NetMessage::GAME_OBJ_DELETE);
		buffer.write(id);
		Network::broadcast(buffer);
	}
}

/*
 * Getter for the teams struct created
 */
MenuOptions GameEngine::getTeams() {
	return teams;
}

/*
 * This function takes in a MenuOptions struct that will replace the 
 * current "teams" struct (this function is called when a player makes
 * a selection for a team in the Server main.cpp file). We check for
 * any conflicts (picking a spot that has already been picked by another
 * player) and will return false if they were caught. It returns true 
 * if there were no conflicts and replaces the teams. 
 */
bool GameEngine::updateMenuOptions(MenuOptions playerMenuOptions) {
	//this is basically a big switcase to check the 4 spots in the struct
	if (teams.team_A_1 != playerMenuOptions.team_A_1) {
		if (teams.team_A_1 != -1) {
			return false;
		}
	}
	if (teams.team_B_1 != playerMenuOptions.team_B_1) {
		if (teams.team_B_1 != -1) {
			return false;
		}
	}
	if (teams.team_A_2 != playerMenuOptions.team_A_2) {
		if (teams.team_A_2 != -1) {
			return false;
		}
	}
	if (teams.team_B_2 != playerMenuOptions.team_B_2) {
		if (teams.team_B_2 != -1) {
			return false;
		}
	}
	//no conflicts were caught
	teams = playerMenuOptions;
	return true;
}

/*
 * This function will assign a default number that can be used for the team
 * placement when making a player. Position returned is 1, 2, 3, 4 in order
 * which maps to team positions A1, B1, A2, B2. 
 */
int GameEngine::nextAvailableSpot(int clientID) {
	//this is basically a big switcase to check the 4 spots in the struct
	if (teams.team_A_1 == -1) {
		teams.team_A_1 = clientID;
		return 1;
	}else if (teams.team_B_1 == -1) {
		teams.team_B_1 = clientID;
		return 2;
	}else if (teams.team_A_2 == -1) {
		teams.team_A_2 = clientID;
		return 3;
	}else if (teams.team_B_2 == -1) {
		teams.team_B_2 = clientID;
		return 4;
	}else {
		//all spots are full, return "spectator" team
		return -1;
	}
}

void GameEngine::init() {
	gameState.in_progress = false;
	gameState.score = std::make_tuple(1, 2);
	gameState.timeLeft = 30;
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

void GameEngine::synchronizeGameState() {
	Network::broadcast(NetMessage::GAME_STATE_UPDATE, gameState);
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
	vector<int> aggregatePlayerMovements(gameState.players.size());
	vector<vec3> directions(gameState.players.size());

	for (PlayerInputs playerInput : playerInputs) {
		aggregatePlayerMovements[playerInput.id] |= playerInput.inputs;
		directions[playerInput.id] = playerInput.direction;
	}

	for (int i = 0; i < gameState.players.size(); i++) {
		auto player = gameState.players[i];
		if (!player) { continue; }
		auto movement = aggregatePlayerMovements[i] & MOVEMENT_MASK;

		player->setDirection(directions[i]);

		// TODO: prevent two players from moving to the same spot
		// gameState.players[i]->move(movementInputToVector(aggregatePlayerMovements[i]));
		noCollisionMove(player, movementInputToVector(movement));
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
	for (GameObject * gameObject : gameState.gameObjects) {
		if (gameObject && gameObject->deleteOnServerTick()) {
			delete gameObject;
		}
	}
}

void GameEngine::updateGameObjectsOnServerTick() {
	for (GameObject * gameObject : gameState.gameObjects) {
		if (gameObject) {
			gameObject->updateOnServerTick();
		}
	}
}

bool GameEngine::noCollisionMove(GameObject * gameObject, vec3 movement) {
	vec3 destination = gameObject->getMoveDestination(movement);

	//for (GameObject * otherGameObject : gameState.gameObjects) {
	//	if (gameObject != gameObject) {
	//		float distance = glm::distance(destination, otherGameObject->getPosition());
	//		if (distance < (gameObject->getRadius() + otherGameObject->getRadius())) {
	//			return false;
	//		}
	//	}
	//}
	gameObject->setPosition(destination);

	return true;
}

const std::vector<GameObject*> &GameEngine::getGameObjects() const {
	return gameState.gameObjects;
}
