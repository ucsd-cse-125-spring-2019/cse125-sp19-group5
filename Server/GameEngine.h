#pragma once

#include <Shared/CommonStructs.h>
#include <Shared/GameObject.h>
#include <Shared/Player.h>
#include <Shared/Ball.h>
#include <Shared/Wall.h>

#define NUM_PLAYERS 1
#define MOVEMENT_MASK 0x11111
#define COMMAND_MASK 0x1100000

class GameEngine {
public:
	void updateGameState(vector<PlayerInputs> playerInputs);
	GameState & getGameState();
	void addGameObject(Player *player);
	void addGameObject(Ball *ball);
	void addGameObject(Wall *wall);
	vec3 movementInputToVector(Player *player, int movementInput);
	void movePlayers(vector<PlayerInputs> playerInputs);
	void doPlayerCommands(vector<PlayerInputs> playerInputs);
	void doCollisionInteractions();
	void removeDeadObjects();

private:
	GameState gameState;
};