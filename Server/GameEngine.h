#pragma once

#include <Shared/CommonStructs.h>
#define NUM_PLAYERS 1
#define MOVEMENT_MASK 0x11111

class GameEngine {
public:
	void updateGameState(vector<PlayerInputs> playerInputs);
	GameState & getGameState();
	void addGameObject(Player *player);
	void addGameObject(Ball *ball);
	void addGameObject(Wall *wall);
	vec3 movementInputToVector(Player *player, int movementInput);

private:
	GameState gameState;
};