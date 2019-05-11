#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include <chrono>
#include "GameEngine.h"
#include "Networking/Server.h"
#include "main.h"

constexpr auto TICKS_PER_SECOND = 60; // How many updates per second.

int main(int argc, char **argv) {

	//TODO: config file for the port number to be specified in
	Network::init(1234);

	//this is the message data sent right after a new player 
	//has connected to the game
	MenuOptions menuOptions;

	GameEngine gameEngine;
	gameEngine.init();

	auto origin = vec3(0.0f);

	vector<PlayerInputs> playerInputs;

	auto ground = new Wall(origin, origin, 100, 1);
	gameEngine.addGameObject(ground);
	ground->setModel("Models/ground.obj");
	ground->setMaterial("Materials/grass.json");

	// Handle player keyboard/mouse inputs
	auto handlePlayerInput = [&playerInputs](Connection *c, NetBuffer &buffer) {
		PlayerInputs input;
		input.id = c->getId();
		input.inputs = buffer.read<int>();
		input.direction = buffer.read<vec3>();
		playerInputs.push_back(input);
	};

	//handle menu option selections made by the player
	//TODO: need to verify this works. ran out of time today
	auto handleMenuInput = [&menuOptions](Connection *c, NetBuffer &playerSelection){
		int player_id = c->getId();
		for (int i = 0; i < 4; i++) {
			if (player_id == playerSelection.read<int>()) {
				int* optionsArray = (int*)&menuOptions;
				if (optionsArray[i] != -1) {
					//this position has already been taken
					NetBuffer pickAgain(NetMessage::MENU_OPTIONS);
					pickAgain.write<MenuOptions>(menuOptions);
					c->send(pickAgain);
				}
				else
				{
					optionsArray[i] = player_id;
				}
			}
		}
	};

	Network::onClientConnected([&](Connection *c) {
		std::cout << "Player " << c->getId() << " has connected." << std::endl;

		// Send Client the connection/player ID 
		NetBuffer buffer(NetMessage::CONNECTION_ID);
		buffer.write<int>(c->getId());
		c->send(buffer);

		// send the menu options available for the game
		NetBuffer menu_options(NetMessage::MENU_OPTIONS);
		menu_options.write<MenuOptions>(menuOptions);
		c->send(menu_options);

		for (auto gameObject : gameEngine.getGameObjects()) {
			if (!gameObject) { continue; }
			NetBuffer buffer(NetMessage::GAME_OBJ_CREATE);
			buffer.write<GAMEOBJECT_TYPES>(gameObject->getGameObjectType());
			gameObject->serialize(buffer);
			c->send(buffer);

			std::cout << "Sent object " << gameObject->getId() << " to " << c->getId() << std::endl;

			NetBuffer modelBuffer(NetMessage::GAME_OBJ_MODEL);
			modelBuffer.write(gameObject->getId());
			modelBuffer.write(gameObject->getModel());
			c->send(modelBuffer);

			NetBuffer animBuffer(NetMessage::GAME_OBJ_ANIM);
			animBuffer.write(gameObject->getId());
			animBuffer.write(gameObject->getAnimation());
			animBuffer.write(true);
			c->send(animBuffer);

			NetBuffer matBuffer(NetMessage::GAME_OBJ_MAT);
			matBuffer.write(gameObject->getId());
			matBuffer.write(gameObject->getMaterial());
			c->send(matBuffer);
		}

		auto player = new Player(origin, origin, c->getId(), 1.0f);
		gameEngine.addGameObject(player);

		player->setModel("Models/player.obj");
		player->setDirection(vec3(0, 0, -1));
		player->setMaterial("Materials/brick.json");

		//Here we handle the menu selections made by the player
		c->on(NetMessage::MENU_INPUT, handleMenuInput);

		// Receive player keyboard and mouse(TODO) input
		c->on(NetMessage::PLAYER_INPUT, handlePlayerInput);
		c->onDisconnected([&](Connection *c) {
			gameEngine.onPlayerDisconnected(c);
			std::cout << "Player " << c->getId() << " has disconnected."
				<< std::endl;
		});
	});

	//This is the total amount of time allowed for the server to update the game state
	auto maxAllowabeServerTime = std::chrono::milliseconds(1000 / TICKS_PER_SECOND + 10);

	//now the game loop begins
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
		if (updateDuration <= maxAllowabeServerTime) 
		{
			//wait for the update time to broadcast the game state update
			std::this_thread::sleep_for(maxAllowabeServerTime - totalDuration);
		}
		else 
		{
			//server has taken too long to process the update!
			//std::cerr << "SERVER TOOK TOO LONG TO UPDATE!" << endl;
		}
	
		gameEngine.synchronizeGameState();
	}

	return 0;
}