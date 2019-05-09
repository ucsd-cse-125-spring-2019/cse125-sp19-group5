#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include <chrono>
#include "GameEngine.h"
#include "Networking/Server.h"

constexpr auto TICKS_PER_SECOND = 60; // How many updates per second.

int main(int argc, char **argv) {
	Network::init(1234);

	GameEngine gameEngine;
	gameEngine.init();

	auto origin = vec3(0.0f);

	vector<PlayerInputs> playerInputs;

	// Handle player keyboard/mouse inputs
	auto handlePlayerInput = [&playerInputs](Connection *c, NetBuffer &buffer) {
		PlayerInputs input;
		auto inputTuple = buffer.read< tuple<int,float,float> >();
		input.id = c->getId();
		input.inputs = std::get<0>(inputTuple);
		input.theta = std::get<1>(inputTuple);
		input.phi = std::get<2>(inputTuple);
		playerInputs.push_back(input);
	};

	Network::onClientConnected([&](Connection *c) {
		std::cout << "Player " << c->getId() << " has connected." << std::endl;

		// Sync up the current state of ballX.
		/*
		NetBuffer buffer(NetMessage::GAME_STATE_UPDATE);
		buffer.write<float>(ballX);
		c->send(buffer);
		*/

		// Send Client the connection/player ID 
		NetBuffer buffer(NetMessage::CONNECTION_ID);
		buffer.write<int>(c->getId());
		c->send(buffer);

		for (auto gameObject : gameEngine.getGameObjects()) {
			if (!gameObject) { continue; }
			NetBuffer buffer(NetMessage::GAME_OBJ_CREATE);
			buffer.write<GAMEOBJECT_TYPES>(gameObject->getGameObjectType());
			gameObject->serialize(buffer);
			c->send(buffer);
			std::cout << "sent object " << gameObject->getId() << " to " << c->getId() << std::endl;
		}

		auto player = new Player(origin, origin, c->getId(), 1.0f);
		player->setDirection(vec3(0, 0, -1));
		player->setScale(vec3(0.2f));
		gameEngine.addGameObject(player);

		// Receive player keyboard and mouse(TODO) input
		c->on(NetMessage::PLAYER_INPUT, handlePlayerInput);
		c->onDisconnected([&](Connection *c) {
			gameEngine.onPlayerDisconnected(c);
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
		//vector<PlayerInputs> playerInputs;
			/*TODO: use the player input (Oliver)*/
			gameEngine.updateGameState(playerInputs);
			playerInputs.clear();

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
	
		gameEngine.synchronizeGameState();
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