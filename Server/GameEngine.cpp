#include "GameEngine.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Networking/Server.h"
#include <Shared/CollisionDetection.h>
#include <Shared/Game/ParticleEmitter.h>
#include <Shared/Util/CurTime.h>
#include "MapLoader.h"

constexpr auto COUNTDOWN_TIME = 3;
constexpr auto ROUND_SCORE_TIME = 3;
constexpr auto SCORE_SHOW_TIME = 10;

#define _DEBUG

template<class T, class V>
void inline safeRemoveFromVec(std::vector<T> &v, V &val) {
	auto it = std::find(v.begin(), v.end(), val);
	if (it != v.end()) {
		v.erase(it);
	}
}

void GameEngine::setGameText(const std::string &newText) {
	NetBuffer buffer(NetMessage::GAME_TEXT);
	buffer.write(newText);
	Network::broadcast(buffer);
	curGameText = newText;
}

void GameEngine::syncGameText(Connection *c) {
	NetBuffer buffer(NetMessage::GAME_TEXT);
	buffer.write(curGameText);
	c->send(buffer);
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

void GameEngine::removeGameObject(GameObject *obj) {
	removeGameObjectById(obj->getId());
}

void GameEngine::init() {
	gameState.in_progress = false;
	gameState.score = std::make_tuple(0, 0);
	gameState.timeLeft = 0;

	setGameText("Waiting for players...");
}

bool GameEngine::shouldGameStart() {
	if (roundState != RoundState::READY) { return false; }
#ifdef _DEBUG_SP
	return gameState.players.size() > 0;
#endif
	if (roundState != RoundState::READY) {
		return false;
	}
	return teamsReady;
}

void GameEngine::prepRound() {
	roundState = RoundState::COUNTDOWN;
	std::cout << "Round starting soon..." << std::endl;

	spawnPlayers();
	setHUDVisible(true);
	// TODO: move ball(s) to spawn
	// TODO: show countdown on screen

	for (int i = 0; i < COUNTDOWN_TIME; i++) {
		auto text = "Starting in " + std::to_string(COUNTDOWN_TIME - i) + "...";
		setTimer("cd" + i, i, [this, text]() {
			setGameText(text);
		});
	}

	setTimer("startRound", COUNTDOWN_TIME, [&]() {
		std::cout << "Round started!" << std::endl;
		setGameText("");
		gameState.in_progress = true;
		roundState = RoundState::ACTIVE;
	});
}

void GameEngine::startGame() {
	std::cout << "Starting a new game..." << std::endl;
	prepRound();
	gameState.score = std::make_tuple(0, 0);

#ifdef _DEBUG_SP
	gameState.timeLeft = 100000 * 10; // a long time
#else
	gameState.timeLeft = 1000 * 60 * 5; // 5 minutes in ms
#endif

	NetBuffer start(NetMessage::START);
	start.write<bool>(true);
	Network::broadcast(start);
}

void GameEngine::endGame() {
	std::cout << "Game over, showing scores..." << std::endl;
	gameState.in_progress = false;
	roundState = RoundState::SHOWING_SCORES;

	showScoreboard();

	setTimer("scores", SCORE_SHOW_TIME, [&]() {
		cleanMap();
		std::cout << "Game over, back to lobby" << std::endl;
		hideScoreboard();
		// TODO: move this later to when teams are finalized
		roundState = RoundState::READY;
		teamsReady = false;
		NetBuffer resetbuf(NetMessage::RESET);
		Network::broadcast(resetbuf);
		NetBuffer teambuf(NetMessage::TEAM);
		teambuf.write<int>(0);
		Network::broadcast(teambuf);
	});
}

void GameEngine::onGoalScored(int team) {
	std::cout << std::get<0>(gameState.score) << " " << std::get<1>(gameState.score) << std::endl;

	// TODO: show GUI stuff for winner
	gameState.in_progress = false;
	roundState = RoundState::TEAM_SCORED;

	auto text = "Team " + std::to_string(team + 1) + " has scored!";
	std::cout << text << std::endl;
	setGameText(text);
	
	setTimer("roundScore", ROUND_SCORE_TIME, [&]() {
		prepRound();
		setGameText("");
	});
}

void GameEngine::onPlayerDisconnected(Connection *c) {
	removeGameObjectById(c->getId());
}

void GameEngine::updateGameState(vector<PlayerInputs> & playerInputs) {
	updateTimers();

	if (!gameState.in_progress) {
		if (shouldGameStart()) {
			startGame();
		}
		return;
	}
	gameState.timeLeft -= PhysicsEngine::getDeltaTime();
	if (gameState.timeLeft <= 0) {
		endGame();
		return;
	}

	movePlayers(playerInputs);
	doPlayerCommands(playerInputs);

	moveBalls();

	// doCollisionInteractions();
	updateScore();
	updateGameObjectsOnServerTick();
	removeDeadObjects();
	spawnItems();

	ParticleEmitter::updateAll();

	// send getNetworkGameState() to client
}

void GameEngine::updateTeamReady() {

	tuple<int, int> temp;
	bool t_ready;
	NetBuffer ready(NetMessage::READY);
	NetBuffer team(NetMessage::TEAM);

	team.write<int>(player_team.size());
	for (auto it = player_team.begin(); it != player_team.end(); it++) {
		team.write(it->first);
		team.write(it->second);
	}

	Network::broadcast(team);

	if (teamR == 1 && teamB == 1 && readyPlayers == 4) {
		t_ready = true;
	}
	else {
		t_ready = false;
	}

	ready.write<bool>(t_ready);
	Network::broadcast(ready);
	if (t_ready) {
		setTimer("start", 3, [&] {
			teamsReady = true;
		});
	}
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

	obj->onCreated();
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

void GameEngine::addGameObject(GameObject *obj) {
	addGenericGameObject(obj);
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
		if (aggregatePlayerMovements.size() <= playerInput.id) {
			continue;
		}
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
	//float diameter = ball->getBoundingSphere()->getRadius() * 2.0f;
	float diameter = 0.2f;

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

		for (Player * player : gameState.players) {
			for (Wall * wall : gameState.walls) {
				if (player->collidesWith(wall)) {
					player->onCollision(wall);
					wall->onCollision(player);
				}
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

	// Process player commands
	for (int i = 0; i < gameState.players.size(); i++) {
		gameState.players[i]->processCommand(aggregatePlayerCommands[i]);
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

	auto team1Scored = std::get<0>(gameState.score) != team1Score;
	auto team2Scored = std::get<1>(gameState.score) != team2Score;
	if (team1Scored) {
		onGoalScored(0);
	}
	if (team2Scored) {
		onGoalScored(1);
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

bool GameEngine::noCollisionMove(Player * player, vec3 movement) {
	vec3 currPosition = player->getPosition();
	// float diameter = player->getBoundingSphere()->getRadius() * 2;
	float diameter = 0.2f;

	vec3 move = player->getMoveDestination(movement) - player->getPosition();
	float dist = glm::length(move);

	while (dist > 0.0f) {
		if (dist > diameter) {
			player->setPosition(player->getPosition() + glm::normalize(move) * diameter);
			dist -= diameter;
		}
		else {
			player->setPosition(player->getPosition() + glm::normalize(move) * dist);
			dist = 0.0f;
		}

		/*for (Player * p : gameState.players) {
			if (player->collidesWith(p)) {
				player->setPosition(currPosition);
				return false;
			}
		}*/

		for (Ball * ball : gameState.balls) {
			if (player->collidesWith(ball)) {
				player->onCollision(ball);
				ball->onCollision(player);
			}
		}

		for (Wall * wall : gameState.walls) {
			if (player->collidesWith(wall)) {
				player->onCollision(wall);
				wall->onCollision(player);
			}
		}

		currPosition = player->getPosition();
	}

	return true;
}

void GameEngine::spawnItems() {
	if (itemTimer == 0) {
		PowerUpItem * powerUpItem = addGameObject<PowerUpItem>();
		powerUpItem->setPowerUpType(POWERUP_TYPES(rand() % NUM_POWERUPS));
		powerUpItem->setBoundingShape(new BoundingSphere(vec3(0, 3, 0), 3.0f));
		powerUpItem->setScale(vec3(3));
		powerUpItem->setModel("Models/unit_sphere.obj");
		powerUpItem->setMaterial("Materials/grass.json");

		tuple<float, float> xRange = std::make_tuple(-50.0f, 50.0f);
		tuple<float, float> yRange = std::make_tuple(70.0f, 80.0f);
		tuple<float, float> zRange = std::make_tuple(-50.0f, 50.0f);
		float xPos = (rand() / (float)RAND_MAX * (std::get<1>(xRange) - std::get<0>(xRange))) + std::get<0>(xRange);
		float yPos = (rand() / (float)RAND_MAX * (std::get<1>(yRange) - std::get<0>(yRange))) + std::get<0>(yRange);
		float zPos = (rand() / (float)RAND_MAX * (std::get<1>(zRange) - std::get<0>(zRange))) + std::get<0>(zRange);

		powerUpItem->setPosition(vec3(xPos, yPos, zPos));
		itemTimer = 900;
	}
	else {
		itemTimer--;
	}
}

const std::array<GameObject*, MAX_GAME_OBJS> &GameEngine::getGameObjects() const {
	return gameState.gameObjects;
}

void GameEngine::onPlayerReady(Connection *c, NetBuffer &buffer) {
	if (gameState.in_progress) { return; }
	auto id = c->getId();
	auto it = readyPlayers.find(id);
	if (it == readyPlayers.end()) {
		readyPlayers.emplace(id);
	} else {
		readyPlayers.erase(it);
	}

}

void GameEngine::setTimer(
	const string &id,
	float time,
	TimerCallback callback
) {
	auto it = timers.find(id);
	if (it != timers.end()) {
		auto timer = it->second;
		timer->expire = curTime() + time;
		timer->onExpire = callback;
		return;
	}
	timers[id] = new Timer{ curTime() + time, callback };
}

void GameEngine::spawnPlayers() {
	auto playerIt = gameState.players.begin();
	for (auto &spawnInfo : spawns) {
		auto team = spawnInfo.first;
		auto position = spawnInfo.second;
		// TODO: team checks
		if (playerIt != gameState.players.end()) {
			(*playerIt)->setPosition(position);
			playerIt++;
		}
	}
}

void GameEngine::spawnBalls() {
}

void GameEngine::updateTimers() {
	auto it = timers.begin();
	auto time = curTime();
	while (it != timers.end()) {
		auto timer = it->second;
		if (timer->expire <= time) {
			timer->onExpire();
			it = timers.erase(it);
		} else {
			it++;
		}
	}
}

void GameEngine::showScoreboard() {
	NetBuffer showBuf(NetMessage::SCOREBOARD_SHOW);
	Network::broadcast(showBuf);

	// TODO: show scores
}

void GameEngine::hideScoreboard() {
	NetBuffer buffer(NetMessage::SCOREBOARD_HIDE);
	Network::broadcast(buffer);
}

void GameEngine::setHUDVisible(bool isVisible) {
	NetBuffer buffer(NetMessage::HUD_VISIBLE);
	buffer.write(isVisible);
	Network::broadcast(buffer);
}

Player *GameEngine::createPlayer(Connection *c) {
	constexpr vec3 origin(0.0f);

	auto player = new Player(vec3(0, 2, 0), origin, origin, c->getId(), 2, 0);
	player->setCooldown(SWING, std::make_tuple(0, 60));
	player->setCooldown(SHOOT, std::make_tuple(0, 60));
	addGameObject(player);

	player->setModel("Models/AntiDeformBear.fbx");
	player->setDirection(vec3(0, 0, -1));
	player->setMaterial("Materials/brown_bear.json");
	player->setScale(vec3(.2));
	player->setAnimation(0);

	return player;
}

void GameEngine::cleanMap() {
	for (auto gameObject : gameState.gameObjects) {
		if (gameObject) {
			removeGameObject(gameObject);
		}
	}
	ParticleEmitter::deleteAll();

	MapLoader mapLoader(this);
	// TODO: change this to some variable - potentially for map votes
	mapLoader.loadMap("Maps/map_with_goals.json");

	for (auto conn : Network::connections) {
		if (conn) {
			createPlayer(conn);
		}
	}
}

GameEngine *gGameEngine = new GameEngine();

