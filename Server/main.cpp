#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include "GameEngine.h"
#include <chrono>
#include "Networking/Server.h"

constexpr auto TICKS_PER_SECOND = 10; // How many updates per second.

int main(int argc, char **argv) {
	Network::init(1234);

	GameEngine gameEngine;
	GameStateNet gameState;
	auto origin = vec3(0.0f);
	//gameState.gameObjects.push_back(Player(origin, origin, origin, "idk", 1));
	gameState.in_progress = false;
	gameState.score = std::make_tuple(1, 2);
	gameState.timeLeft = 30;
	vector<PlayerInputs> playerInputs;

	float ballX = 0.0f;

	// Handle player requests to move the ball.
	auto handleBallMove = [&](Connection *c, NetBuffer &buffer) {
		ballX += buffer.read<float>();

		// Replicate the changes so everyone sees the update.
		NetBuffer updateBuffer(NetMessage::BALL_X);
		updateBuffer.write<float>(ballX);
		Network::broadcast(updateBuffer);
	};

	// Handle player keyboard/mouse inputs
	auto handlePlayerInput = [&playerInputs](Connection *c, NetBuffer &buffer) {
		PlayerInputs input;
		auto inputTuple = buffer.read< tuple<int,float,float> >();
		input.id = c->getId();
		input.inputs = std::get<0>(inputTuple);
		playerInputs.push_back(input);

		//cout << "handlePlayerInput: " << input.inputs << endl;
	};

	Network::onClientConnected([&ballX, &gameEngine,&handleBallMove, &handlePlayerInput](Connection *c) {
		std::cout << "Player " << c->getId() << " has connected." << std::endl;

		// Sync up the current state of ballX.
		NetBuffer buffer(NetMessage::BALL_X);
		buffer.write<float>(ballX);
		c->send(buffer);

		gameEngine.addGameObject(new Player(vec3(0.0f), vec3(0.0f), vec3(0.0f,0.0f,-1.0f), "p0", 1));

		// Allow the newly connected player to move the ball.
		c->on(NetMessage::BALL_X, handleBallMove);

		// Receive player keyboard and mouse(TODO) input
		c->on(NetMessage::PLAYER_INPUT, handlePlayerInput);
		c->onDisconnected([](Connection *c) {
			std::cout << "Player " << c->getId() << " has disconnected."
				<< std::endl;
		});
	});

	while (true) {
		Network::poll();

		//vector<PlayerInputs> playerInputs;
		//for (int i = 0; i < 4; i++) {
		gameEngine.updateGameState(playerInputs);
		//}
		playerInputs.clear();

		gameState.timeLeft -= 1;

		Network::broadcast(NetMessage::TEST, gameState);

		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000 / TICKS_PER_SECOND)
		);
	}

	return 0;
}