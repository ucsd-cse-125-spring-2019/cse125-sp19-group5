#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include <Shared/BoundingSphere.h>
#include <Shared/BoundingBox.h>
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

	auto ground = gameEngine.addGameObject<Wall>();
	ground->setBoundingShape(new BoundingBox(vec3(0, -1, 0), vec3(1, 0, 0), 1, 1, 100));
	ground->setPosition(vec3(0, -1, 0));
	ground->setModel("Models/ground.obj");
	ground->setMaterial("Materials/grass.json");

	auto ball = gameEngine.addGameObject<Ball>();
	ball->setBoundingShape(new BoundingSphere(origin, 4));
	ball->setScale(vec3(1.0f/4.0f));
	ball->setPosition(vec3(0, 4, 0));
	ball->setVelocity(vec3(0, 0, 0));
	ball->setModel("Models/sphere.obj");
	ball->setMaterial("Materials/brick.json");

	auto wall1 = gameEngine.addGameObject<Wall>();
	wall1->setBoundingShape(new BoundingBox(vec3(0, 0, 100), vec3(1, 0, 0), 200, 20, 10));
	wall1->setScale(vec3(200, 40, 10));
	wall1->setPosition(vec3(0, 0, 100));
	wall1->setModel("Models/unit_cube.obj");
	wall1->setMaterial("Materials/grass.json");

	auto wall2 = gameEngine.addGameObject<Wall>();
	wall2->setBoundingShape(new BoundingBox(vec3(0, 0, -100), vec3(1, 0, 0), 200, 20, 10));
	wall2->setScale(vec3(200, 40, 10));
	wall2->setPosition(vec3(0, 0, -100));
	wall2->setModel("Models/unit_cube.obj");
	wall2->setMaterial("Materials/grass.json");

	auto wall3 = gameEngine.addGameObject<Wall>();
	wall3->setBoundingShape(new BoundingBox(vec3(100, 0, 0), vec3(0, 0, 1), 200, 20, 10));
	wall3->setScale(vec3(10, 40, 200));
	wall3->setPosition(vec3(100, 0, 0));
	wall3->setModel("Models/unit_cube.obj");
	wall3->setMaterial("Materials/grass.json");

	auto wall4 = gameEngine.addGameObject<Wall>();
	wall4->setBoundingShape(new BoundingBox(vec3(-100, 0, 0), vec3(0, 0, 1), 200, 20, 10));
	wall4->setScale(vec3(10, 40, 200));
	wall4->setPosition(vec3(-100, 0, 0));
	wall4->setModel("Models/unit_cube.obj");
	wall4->setMaterial("Materials/grass.json");




	// Handle player keyboard/mouse inputs
	auto handlePlayerInput = [&playerInputs](Connection *c, NetBuffer &buffer) {
		PlayerInputs input;
		input.id = c->getId();
		input.inputs = buffer.read<int>();
		input.direction = buffer.read<vec3>();
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

		auto player = new Player(origin, origin, origin, c->getId(), 1.0f, 0);
		gameEngine.addGameObject(player);

		player->setModel("Models/player.obj");
		player->setDirection(vec3(0, 0, -1));
		player->setMaterial("Materials/brick.json");

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
}