#include <iostream>
#include "GameEngine.h"

int main(int argc, char **argv) {
	std::cout << "Hello world!" << std::endl;
	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(-2, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), "player_0", 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), "ball_0", 1));

	vector<PlayerInputs> playerInputs;
	PlayerInputs pi;
	pi.id = 0;
	pi.inputs = SWING;
	playerInputs.push_back(pi);

	vector<PlayerInputs> noInputs;
	pi.id = 0;
	pi.inputs = 0;
	noInputs.push_back(pi);

	for (int i = 0; i < 15; i++) {
		if (i < 2) {
			gameEngine.updateGameState(playerInputs);
		}
		else {
			gameEngine.updateGameState(noInputs);

		}
	}

	system("pause");
	return 0;
}