#pragma once

#include <Shared/Common.h>

class GameEngine {
public:
	void updateGameState();
	GameState & getGameState();

private:
	GameState gameState;
};