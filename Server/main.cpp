#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include "GameEngine.h"
#include <chrono>
#include "Networking/Server.h"

constexpr auto TICKS_PER_SECOND = 60; // How many updates per second.

int main(int argc, char **argv) {
	//Network::init(1234);

	//GameEngine gameEngine;
	//GameStateNet gameState;
	//auto origin = vec3(0.0f);
	//gameState.gameObjects.push_back(Player(origin, origin, origin, "idk", 1));
	//gameState.in_progress = false;
	//gameState.score = std::make_tuple(1, 2);
	//gameState.timeLeft = 30;

	//float ballX = 0.0f;

	//// Handle player requests to move the ball.
	//auto handleBallMove = [&](Connection *c, NetBuffer &buffer) {
	//	ballX += buffer.read<float>();

	//	// Replicate the changes so everyone sees the update.
	//	NetBuffer updateBuffer(NetMessage::BALL_X);
	//	updateBuffer.write<float>(ballX);
	//	Network::broadcast(updateBuffer);
	//};

	//Network::onClientConnected([&ballX, &handleBallMove](Connection *c) {
	//	std::cout << "Player " << c->getId() << " has connected." << std::endl;

	//	// Sync up the current state of ballX.
	//	NetBuffer buffer(NetMessage::BALL_X);
	//	buffer.write<float>(ballX);
	//	c->send(buffer);

	//	// Allow the newly connected player to move the ball.
	//	c->on(NetMessage::BALL_X, handleBallMove);

	//	c->onDisconnected([](Connection *c) {
	//		std::cout << "Player " << c->getId() << " has disconnected."
	//			<< std::endl;
	//	});
	//});

	//while (true) {
	//	Network::poll();

	//	vector<PlayerInputs> playerInputs;
	//	for (int i = 0; i < 4; i++) {
	//		gameEngine.updateGameState(playerInputs);
	//	}

	//	gameState.timeLeft -= 1;

	//	Network::broadcast(NetMessage::TEST, gameState);

	//	std::this_thread::sleep_for(
	//		std::chrono::milliseconds(1000 / TICKS_PER_SECOND)
	//	);
	//}



	// testing code
	std::cout << "Hello world!" << std::endl;
	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(-2, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), 0, 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), 0, 1));

	vector<PlayerInputs> playerInputs;
	PlayerInputs pi;
	pi.id = 0;
	pi.inputs = SWING + RIGHT + LEFT;
	playerInputs.push_back(pi);

	vector<PlayerInputs> noInputs;
	pi.id = 0;
	pi.inputs = 0;
	noInputs.push_back(pi);

	for (int i = 0; i < 15; i++) {
		if (i < 3) {
			gameEngine.updateGameState(playerInputs);
		}
		else {
			gameEngine.updateGameState(noInputs);

		}
	}

	system("pause");

	return 0;
}