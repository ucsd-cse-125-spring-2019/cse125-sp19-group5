#pragma once

#include <Shared/CommonStructs.h>
#include <Shared/GameObject.h>
#include <Shared/Player.h>
#include <Shared/Ball.h>
#include <Shared/Bullet.h>
#include <Shared/Wall.h>
#include <Shared/Goal.h>
#include <Shared/GameState.h>
#include <Shared/Networking/Connection.h>
#include <unordered_set>
#include <functional>
#include <unordered_map>

#define NUM_PLAYERS 1
#define MOVEMENT_MASK 0b11111

using TimerCallback = std::function<void()>;

enum class RoundState {
	READY,
	COUNTDOWN,
	ACTIVE,
	SHOWING_SCORES,
	TEAM_SCORED,
};

struct Timer {
	float expire;
	TimerCallback onExpire;
};

class GameEngine {
public:
	void init();

	void onPlayerDisconnected(Connection *c);

	void updateGameState(vector<PlayerInputs> & playerInputs);
	void updateTeamReady(unordered_map<int, int> p_t, int teamR, int teamB);
	void synchronizeGameState();
	GameState & getGameState();
	
	template<class T>
	T* addGameObject() {
		auto id = gameState.getFreeId();
		auto obj = new T(id);
		addGameObject(obj);
		return obj;
	}

	void addGameObject(Player *player);
	void addGameObject(Ball *ball);
	void addGameObject(Wall *wall);
	void addGameObject(Goal * goal);
	void addGameObject(GameObject *obj);
	void removeGameObjectById(int id);

	vec3 movementInputToVector(int movementInput);
	void movePlayers(vector<PlayerInputs> & playerInputs);

	void incrementalMoveBall(Ball * ball, float dist);
	void moveBalls();

	void doPlayerCommands(vector<PlayerInputs> & playerInputs);

	void doCollisionInteractions();
	void updateScore();
	void removeDeadObjects();
	void updateGameObjectsOnServerTick();
	bool noCollisionMove(Player * player, vec3 movement);

	const std::array<GameObject*, MAX_GAME_OBJS> &getGameObjects() const;

	void onPlayerReady(Connection *c, NetBuffer &buffer);

	void setTimer(const string &id, float time, TimerCallback callback);

	std::vector<std::pair<int, vec3>> spawns;
	void spawnPlayers();

	void spawnBalls();

	void setGameText(const string &newText);
	void syncGameText(Connection *c);
private:
	std::unordered_map<string, Timer*> timers;
	std::unordered_set<int> readyPlayers;
	GameState gameState;
	RoundState roundState = RoundState::READY;
	string curGameText = "";

	void addGenericGameObject(GameObject *player);
	bool shouldGameStart();
	void startGame();
	void endGame();
	void onGoalScored(int team);
	void prepRound();

	void updateTimers();
};

extern GameEngine *gGameEngine;
