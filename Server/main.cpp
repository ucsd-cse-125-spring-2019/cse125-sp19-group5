#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include <Shared/PhysicsEngine.h>
#include <Shared/BoundingSphere.h>
#include <Shared/BoundingBox.h>
#include <chrono>
#include "GameEngine.h"
#include "Networking/Server.h"
#include <Shared/Game/ParticleEmitter.h>
#include "MapLoader.h"
#include "Game/Powerups/SpeedBoost.h"

constexpr auto TICKS_PER_SECOND = 60; // How many updates per second.

int main(int argc, char **argv) {
	Network::init(1234);

	GameEngine &gameEngine = *gGameEngine;
	gameEngine.init();

	auto origin = vec3(0.0f);

	vector<PlayerInputs> playerInputs;
  
	MapLoader mapLoader(&gameEngine);
	mapLoader.loadMap("Maps/map_with_goals.json");

	/*auto goal1 = gameEngine.addGameObject<Goal>();
	goal1->setBoundingShape(new BoundingBox(vec3(0, 0, 100), vec3(1, 0, 0), 200, 20, 10));
	goal1->setScale(vec3(200, 40, 10));
	goal1->setPosition(vec3(0, 0, 100));
	goal1->setTeam(0);
	goal1->setModel("Models/unit_cube.obj");
	goal1->setMaterial("Materials/brick.json");

	auto goal2 = gameEngine.addGameObject<Goal>();
	goal2->setBoundingShape(new BoundingBox(vec3(0, 0, -100), vec3(1, 0, 0), 200, 20, 10));
	goal2->setScale(vec3(200, 40, 10));
	goal2->setPosition(vec3(0, 0, -100));
	goal2->setTeam(1);
	goal2->setModel("Models/unit_cube.obj");
	goal2->setMaterial("Materials/brick.json");*/
	
	/*auto wall5 = gameEngine.addGameObject<Wall>();
	wall5->setBoundingShape(new BoundingBox(vec3(-20, 0, 10), vec3(1, 0, 1), 50, 20, 5));
	wall5->setScale(vec3(50, 40, 5));
	wall5->setPosition(vec3(-20, 0, 10));
	wall5->setModel("models/unit_cube.obj");
	wall5->setMaterial("materials/grass.json");

	auto quat4 = quat(vec3(0, glm::radians(-45.0f), 0));
	wall5->setOrientation(quat4);*/


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

		auto player = new Player(vec3(0, 2, 0), origin, origin, c->getId(), 2, 0);
		player->setCooldown(SWING, std::make_tuple(0, 60));
		player->setCooldown(SHOOT, std::make_tuple(0, 60));
		gameEngine.addGameObject(player);

		player->setModel("Models/unit_sphere.obj");
		player->setDirection(vec3(0, 0, -1));
		player->setMaterial("Materials/brick.json");
		player->setScale(vec3(2));

		auto ps = new ParticleEmitter();
		ps->setGravity(-15.0f);
		ps->setCreationSpeed(500);
		ps->setInitialVel(vec3(0, 10, 0));
		ps->setTexture("Textures/gary.png");

		// Receive player keyboard and mouse(TODO) input
		c->on(NetMessage::PLAYER_INPUT, handlePlayerInput);
		c->onDisconnected([&](Connection *c) {
			gameEngine.onPlayerDisconnected(c);
			std::cout << "Player " << c->getId() << " has disconnected."
				<< std::endl;
		});
	});

	//This is the total amount of time allowed for the server to update the game state
	auto maxAllowableServerTime = std::chrono::milliseconds(1000 / TICKS_PER_SECOND + 10);

	// Time step used for physics
	PhysicsEngine::setDeltaTime(maxAllowableServerTime.count());

	while (true) 
	{

		//time keeping stuff to know how long the server has been running for
		auto startTime = std::chrono::high_resolution_clock::now();
		Network::poll();//checking the network for client update messages
		auto pollDone = std::chrono::high_resolution_clock::now();
		auto networkPollDuration = std::chrono::duration_cast<std::chrono::milliseconds>(pollDone - startTime);

		//updating the game state with each client message
		gameEngine.updateGameState(playerInputs);
		playerInputs.clear();

		//timekeeping stuff to check the duration so far
		auto updateDone = std::chrono::high_resolution_clock::now();
		auto updateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(updateDone - pollDone);
		auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(updateDone - startTime);

		//check if the server is running on schedule
		if (updateDuration <= maxAllowableServerTime) 
		{
			//wait for the update time to broadcast the game state update
			std::this_thread::sleep_for(maxAllowableServerTime - totalDuration);
		}
		else 
		{
			//server has taken too long to process the update!
			//std::cerr << "SERVER TOOK TOO LONG TO UPDATE!" << endl;
		}
	
		gameEngine.synchronizeGameState();
	}
}
