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

#define NUM_PLAYERS 1
#define MOVEMENT_MASK 0b11111
#define COMMAND_MASK 0b11100000

class GameEngine {
public:
	void init();

	void onPlayerDisconnected(Connection *c);

	void updateGameState(vector<PlayerInputs> & playerInputs);
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
	bool noCollisionMove(GameObject * gameObject, vec3 movement);

	const vector<GameObject*> &getGameObjects() const;

private:
	GameState gameState;
	void addGenericGameObject(GameObject *player);
};