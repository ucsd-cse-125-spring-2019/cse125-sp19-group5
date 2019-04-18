#include "GameEngine.h"

void GameEngine::updateGameState(vector<PlayerInputs> playerInputs) {
	// all players move
	for (Player *player : gameState.players) {
		player->setPosition(player->getPosition() + player->getVelocity());
	}

	// check collisions
	for (GameObject * gameObject1 : gameState.gameObjects) {
		for (GameObject * gameObject2 : gameState.gameObjects) {
			if (gameObject1->collidesWith(*gameObject2)) {
				gameObject1->onCollision(*gameObject2);
			}
		}
	}
}

GameState & GameEngine::getGameState() {
	return gameState;
}

void GameEngine::addGameObject(Player *player) {
	gameState.gameObjects.push_back(player);
	gameState.players.push_back(player);
}

void GameEngine::addGameObject(Ball *ball) {
	gameState.gameObjects.push_back(ball);
	gameState.balls.push_back(ball);
}

void GameEngine::addGameObject(Wall *wall) {
	gameState.gameObjects.push_back(wall);
	gameState.walls.push_back(wall);
}
