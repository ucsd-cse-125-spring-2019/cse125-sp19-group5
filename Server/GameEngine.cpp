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
		safeRemoveFromVec(gameState.goals, gameObject);
		delete gameObject;

		NetBuffer buffer(NetMessage::GAME_OBJ_DELETE);
		buffer.write(id);
		Network::broadcast(buffer);
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

	// doCollisionInteractions();
	updateScore();
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
	std::cout << obj->to_string() << std::endl;
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

void GameEngine::addGameObject(Goal *goal) {
	addGenericGameObject(goal);
	gameState.goals.push_back(goal);
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
	if (movementInput & JUMP) {
		movement = movement + vec3(0, 1, 0);
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

void GameEngine::incrementalMoveBall(Ball * ball, float dist) {
	float diameter = ball->getBoundingSphere()->getRadius() * 2.0f;

	while (dist > 0.0f && !ball->getGoalScored()) {
		if (dist > diameter) {
			ball->move(glm::normalize(ball->getVelocity()) * diameter);
			dist -= diameter;
		}
		else {
			ball->move(glm::normalize(ball->getVelocity()) * dist);
			dist = 0.0f;
		}

		// check to see if ball collides with goal or wall
		for (Goal * goal : gameState.goals) {
			if (ball->collidesWith(goal)) {
				ball->onCollision(goal);
				goal->onCollision(ball);
			}
		}

		for (Wall * wall : gameState.walls) {
			if (ball->collidesWith(wall)) {
				ball->onCollision(wall);
				wall->onCollision(ball);
			}
		}
	}
}

void GameEngine::moveBalls() {
	// TODO: manage ballUnitMoves in ball object so balls can take each other's velocity
	// TODO: account for balls with 0 velocity
	// TODO: move balls in increments based on their velocity
	int moves_per_tick = 1;

	doCollisionInteractions();

	// move all balls their incremental move while checking for wall collisions
	for (int i = 0; i < gameState.balls.size(); i++) {
		Ball * ball = gameState.balls[i];
		if (ball) {
			for (int j = 0; j < moves_per_tick; j++) {
				float moveDist = glm::length(ball->getVelocity()) / moves_per_tick;
				incrementalMoveBall(ball, moveDist);
			}
		}
		// check for other collisions at end
		doCollisionInteractions();
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
			createdGameObject->setModel("Models/unit_sphere.obj");
			createdGameObject->setMaterial("Materials/brick.json");
		}
	}
}

void GameEngine::doCollisionInteractions() {
	for (int i = 0; i < gameState.gameObjects.size(); i++) {
		GameObject * gameObject1 = gameState.gameObjects[i];
		if (!gameObject1) { continue; }
		for (int j = i + 1; j < gameState.gameObjects.size(); j++) {
			GameObject * gameObject2 = gameState.gameObjects[j];
			if (gameObject2 && gameObject1->collidesWith(gameObject2)) {
				gameObject1->onCollision(gameObject2);
				gameObject2->onCollision(gameObject1);
			}
		}
	}
}

void GameEngine::updateScore() {
	int team1Score = 0;
	int team2Score = 0;
	for (Goal * goal : gameState.goals) {
		if (goal->getTeam() == 0) {
			team1Score += goal->getGoalsScored();
		}
		else {
			team2Score += goal->getGoalsScored();
		}
	}

	if (std::get<0>(gameState.score) != team1Score || std::get<1>(gameState.score) != team2Score) {
		std::cout << team1Score << " " << team2Score << std::endl;
	}

	std::get<0>(gameState.score) = team1Score;
	std::get<1>(gameState.score) = team2Score;
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
	// TODO: fix this method by moving adding a bounding box where the object would move

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
