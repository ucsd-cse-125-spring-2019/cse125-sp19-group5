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
	GameStateNet gameState;
	auto origin = vec3(0.0f);
	gameState.gameObjects.push_back(Player(origin, origin, origin, 0, 1));
	gameState.in_progress = false;
	gameState.score = std::make_tuple(1, 2);
	gameState.timeLeft = 30;

	float ballX = 0.0f;

	// Handle player requests to move the ball.
	auto handleBallMove = [&](Connection *c, NetBuffer &buffer) {
		ballX += buffer.read<float>();

		// Replicate the changes so everyone sees the update.
		NetBuffer updateBuffer(NetMessage::BALL_X);
		updateBuffer.write<float>(ballX);
		Network::broadcast(updateBuffer);
	};

	Network::onClientConnected([&ballX, &handleBallMove](Connection *c) {
		std::cout << "Player " << c->getId() << " has connected." << std::endl;

		// Sync up the current state of ballX.
		/*
		NetBuffer buffer(NetMessage::GAME_STATE_UPDATE);
		buffer.write<float>(ballX);
		c->send(buffer);
		*/

		// Allow the newly connected player to move the ball.
		c->on(NetMessage::BALL_X, handleBallMove);

		c->onDisconnected([](Connection *c) {
			std::cout << "Player " << c->getId() << " has disconnected."
				<< std::endl;
		});
	});

	//This is the total amount of time allowed for the server to update the game state
	auto maxAllowabeServerTime = std::chrono::milliseconds(1000 / TICKS_PER_SECOND);

	while (true) 
	{

		//time keeping stuff to know how long the server has been running for
		auto startTime = std::chrono::high_resolution_clock::now();
		Network::poll();//checking the network for client update messages
		auto pollDone = std::chrono::high_resolution_clock::now();
		auto networkPollDuration = std::chrono::duration_cast<std::chrono::milliseconds>(pollDone - startTime);

		//updating the game state with each client message
		vector<PlayerInputs> playerInputs;
		for (int i = 0; i < 4; i++) 
		{
			/*TODO: use the player input (Oliver)*/
			gameEngine.updateGameState(playerInputs);
		}

		//timekeeping stuff to check the duration so far
		auto updateDone = std::chrono::high_resolution_clock::now();
		auto updateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(updateDone - pollDone);
		auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(updateDone - startTime);

		//check if the server is running on schedule
		if (totalDuration < maxAllowabeServerTime) 
		{
			//wait for the update time to broadcast the game state update
			std::this_thread::sleep_for(maxAllowabeServerTime - totalDuration);
		}
		else 
		{
			//server has taken too long to process the update!
			std::cerr << "SERVER TOOK TOO LONG TO UPDATE!" << endl;
		}

		//broadcast the updated game state
		GameStateNet updatedState = gameEngine.getNetworkGameState();
		Network::broadcast(NetMessage::GAME_STATE_UPDATE, updatedState);
	}



	// testing code
	/*std::cout << "Hello world!" << std::endl;
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

	system("pause");*/

	return 0;
}