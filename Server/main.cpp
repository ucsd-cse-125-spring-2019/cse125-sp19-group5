#include <iostream>
#include "GameEngine.h"

int main(int argc, char **argv) {
	std::cout << "Hello world!" << std::endl;
	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(0, 0, 0), vec3(1, 0, 0), "player_1", 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), "ball_1", 1));

	vector<PlayerInputs> playerInputs;
	for (int i = 0; i < 4; i++) {
		gameEngine.updateGameState(playerInputs);
	}

	system("pause");
	return 0;
}