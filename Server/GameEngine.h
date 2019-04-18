#pragma once

#include <Shared/CommonStructs.h>

class GameEngine {
public:
	void updateGameState(vector<PlayerInputs> playerInputs);
	GameState & getGameState();

private:
	GameState gameState;
};