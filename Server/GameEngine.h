#pragma once

#include <Shared/CommonStructs.h>
#include <Shared/GameObject.h>
#include <Shared/Player.h>
#include <Shared/Ball.h>
#include <Shared/Wall.h>
#include <Shared/GameState.h>
#include <Shared/Networking/Connection.h>

#define NUM_PLAYERS 1
#define MOVEMENT_MASK 0b11111
#define COMMAND_MASK 0b1100000

class GameEngine {
public:
	void onPlayerDisconnected(Connection *c);

	void updateGameState(vector<PlayerInputs> & playerInputs);
	GameState & getGameState();
	void addGameObject(Player *player);
	void addGameObject(Ball *ball);
	void addGameObject(Wall *wall);
	void removeGameObjectById(int id);

	vec3 movementInputToVector(int movementInput);
	void movePlayers(vector<PlayerInputs> & playerInputs);
	void moveBalls();
	void doPlayerCommands(vector<PlayerInputs> & playerInputs);

	void doCollisionInteractions();
	void removeDeadObjects();
	void updateGameObjectsOnServerTick();
	GameStateNet * getGameStateNet(GameStateNet * networkGameState);
	bool noCollisionMove(GameObject * gameObject, vec3 movement);

	const vector<GameObject*> &getGameObjects() const;

private:
	GameState gameState;
	void addGenericGameObject(GameObject *player);
};