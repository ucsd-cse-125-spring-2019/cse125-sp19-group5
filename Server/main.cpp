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
	Network::init(1234);

	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), "player_1", 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), "ball_1", 1));

	GameStateNet gameState;
	auto origin = vec3(0.0f);
	gameState.gameObjects.push_back(Player(origin, origin, origin, "idk", 1));
	gameState.in_progress = false;
	gameState.score = std::make_tuple(1, 2);
	gameState.timeLeft = 30;

	Network::onClientConnected([](Connection *c) {
		std::cout << "Player " << c->getId() << " has connected." << std::endl;

		c->onDisconnected([](Connection *c) {
			std::cout << "Player " << c->getId() << " has disconnected.";
		});
	});

	while (true) {
		Network::poll();

		vector<PlayerInputs> playerInputs;
		for (int i = 0; i < 4; i++) {
			gameEngine.updateGameState(playerInputs);
		}

		gameState.timeLeft -= 1;

		Network::broadcast(NetMessage::TEST, gameState);

		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000 / TICKS_PER_SECOND)
		);
	}

	return 0;
}