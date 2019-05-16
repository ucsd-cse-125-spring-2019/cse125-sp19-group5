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
 * This function is for confirming the team placement of the given client. 
 * Since we want the Game Engine to be the single source of truth, this function 
 * was necessary when writing the code in Server/main.cpp to create the new
 * player -- the player needs to be created with a team so we ask the Game
 * Engine what team the player is on (line 53 in main.cpp)
 */
int GameEngine::getTeamOf(int clientID) {
	if (serverMenuOptions.team_A_1 == clientID || serverMenuOptions.team_A_2 == clientID) {
		return 1;//this client is on team 1
	}
	else if (serverMenuOptions.team_B_1 == clientID || serverMenuOptions.team_B_2 == clientID) {
		return 2;//this client is on team 2
	}
	else {
		return -1;//this client is not on a team
	}
}

/*
 * Getter for the teams struct created
 */
MenuOptions GameEngine::getTeams() {
	return serverMenuOptions;
}

/*
 * This function takes in a MenuOptions struct that will replace the 
 * current "teams" struct (this function is called when a player makes
 * a selection for a team in the Server main.cpp file). We check for
 * any conflicts (picking a spot that has already been picked by another
 * player) and will return false if they were caught. It returns true 
 * if there were no conflicts and replaces the teams. 
 */
bool GameEngine::updateMenuOptions(MenuOptions playerMenuOptions) 
{
	/*DEBUG: Printout the updated menu options */
	MenuOptions currentMenuOptions = serverMenuOptions;
	cout << "-------------------------- " << endl;
	cout << "HERE ARE THE CURRENT MENU OPTIONS: " << endl << endl;
	cout << "Team A (1):             " << (int)currentMenuOptions.team_A_1 << endl;
	cout << "Team A (2):             " << (int)currentMenuOptions.team_A_2 << endl << endl;
	cout << "Team B (1):             " << (int)currentMenuOptions.team_B_1 << endl;
	cout << "Team B (2):             " << (int)currentMenuOptions.team_B_2 << endl;
	cout << "-------------------------- " << endl;
	/*END_DEBUG*/

	//this is basically a big switcase to check the 4 spots in the struct
	if (serverMenuOptions.team_A_1 != playerMenuOptions.team_A_1) {
		if (serverMenuOptions.team_A_1 != -1) {
			return false;
		}
	}
	if (serverMenuOptions.team_B_1 != playerMenuOptions.team_B_1) {
		if (serverMenuOptions.team_B_1 != -1) {
			return false;
		}
	}
	if (serverMenuOptions.team_A_2 != playerMenuOptions.team_A_2) {
		if (serverMenuOptions.team_A_2 != -1) {
			return false;
		}
	}
	if (serverMenuOptions.team_B_2 != playerMenuOptions.team_B_2) {
		if (serverMenuOptions.team_B_2 != -1) {
			return false;
		}
	}
	
	/*DEBUG: Printout the updated menu options */
	currentMenuOptions = playerMenuOptions;
	cout << "-------------------------- " << endl;
	cout << "HERE ARE THE UPDATED MENU OPTIONS: " << endl << endl;
	cout << "Team A (1):             " << (int)currentMenuOptions.team_A_1 << endl;
	cout << "Team A (2):             " << (int)currentMenuOptions.team_A_2 << endl << endl;
	cout << "Team B (1):             " << (int)currentMenuOptions.team_B_1 << endl;
	cout << "Team B (2):             " << (int)currentMenuOptions.team_B_2 << endl;
	cout << "-------------------------- " << endl;
	/*END_DEBUG*/

	//no conflicts were caught, accept the update
	serverMenuOptions = playerMenuOptions;
	return true;
}

/*
 * This function will assign a default number that can be used for the team
 * placement when making a player. Position returned is 1, 2, 3, 4 in order
 * which maps to team positions A1, B1, A2, B2. 
 */
int GameEngine::nextAvailableSpot(int clientID) {
	//this is basically a big switcase to check the 4 spots in the struct
	if (serverMenuOptions.team_A_1 == -1) {
		serverMenuOptions.team_A_1 = clientID;
		return 1;
	}else if (serverMenuOptions.team_B_1 == -1) {
		serverMenuOptions.team_B_1 = clientID;
		return 2;
	}else if (serverMenuOptions.team_A_2 == -1) {
		serverMenuOptions.team_A_2 = clientID;
		return 3;
	}else if (serverMenuOptions.team_B_2 == -1) {
		serverMenuOptions.team_B_2 = clientID;
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
			createdGameObject->setId(gameState.getFreeId());
			addGenericGameObject(createdGameObject);
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
			removeGameObjectById(gameObject->getId());
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

int GameEngine::getTeam() {

	int team = -1;

	if (num_players < NUM_PLAYERS) {
		 team = num_players % 2;
		 num_players += 1;
	}
	return team;
}

const std::vector<GameObject*> &GameEngine::getGameObjects() const {
	return gameState.gameObjects;
}


