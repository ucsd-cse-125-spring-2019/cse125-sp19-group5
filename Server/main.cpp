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
#include <unordered_map>

constexpr auto TICKS_PER_SECOND = 60; // How many updates per second.

int main(int argc, char **argv) {
	Network::init(1234);

	GameEngine &gameEngine = *gGameEngine;
	gameEngine.init();

	auto origin = vec3(0.0f);

	vector<PlayerInputs> playerInputs;
  
	MapLoader mapLoader(&gameEngine);
	mapLoader.loadMap("Maps/map_with_goals.json");

	/*
	unordered_map<int, int> player_team;
	unordered_map<int, std::string> id_name;
	int teamR = 0;
	int teamB = 0;*/

	/*auto jumpableBox = gameEngine.addGameObject<Wall>();
	mapLoader.loadMap("Maps/map_with_goals.json");

	unordered_map<int, int> player_team;
	unordered_map<int, std::string> id_name;
	int teamR = 0;
	int teamB = 0;

	auto jumpableBox = gameEngine.addGameObject<Wall>();
	jumpableBox->setBoundingShape(new BoundingBox(vec3(0, 0, 30), vec3(1, 0, 0), 30, 8, 30));
	jumpableBox->setPosition(vec3(0, 0, 30));
	jumpableBox->setScale(vec3(30, 8, 30));
	jumpableBox->setModel("Models/unit_cube.obj");
	jumpableBox->setMaterial("Materials/brick.json");

	auto jumpableBox2 = gameEngine.addGameObject<Wall>();
	jumpableBox2->setBoundingShape(new BoundingBox(vec3(0, 0, 60), vec3(1, 0, 0), 30, 16, 30));
	jumpableBox2->setPosition(vec3(0, 0, 60));
	jumpableBox2->setScale(vec3(30, 16, 30));
	jumpableBox2->setModel("Models/unit_cube.obj");
	jumpableBox2->setMaterial("Materials/brick.json");*/

	// Handle player keyboard/mouse inputs
	auto handlePlayerInput = [&playerInputs](Connection *c, NetBuffer &buffer) {
		PlayerInputs input;
		input.id = c->getId();
		input.inputs = buffer.read<int>();
		input.direction = buffer.read<vec3>();
		playerInputs.push_back(input);
	};

	auto handlePlayerReady = boost::bind(
		&GameEngine::onPlayerReady, &gameEngine,
		_1, _2
	);

	auto handleTeamSelection = [&](Connection*c, NetBuffer &buffer) {
		
		if (gameEngine.player_team.at(c->getId()) == 0) gameEngine.teamR -= 1;
		else gameEngine.teamB -= 1;

		gameEngine.player_team[c->getId()] = buffer.read<int>();

		if (gameEngine.player_team.at(c->getId()) == 0) gameEngine.teamR += 1;
		else gameEngine.teamB += 1;

		gameEngine.updateTeamReady();
	};

	auto addPlayerName = [&](Connection*c, NetBuffer &buffer) {

		std::string name = buffer.read<std::string>();
		gameEngine.id_name[c->getId()] = name;
		NetBuffer id_name(NetMessage::NAME);
		id_name.write(c->getId());
		id_name.write(name);
		Network::broadcast(id_name);

		gameEngine.player_team[c->getId()] = (gameEngine.player_team.size() + 1) % 2;
		if (gameEngine.player_team.at(c->getId()) == 0) gameEngine.teamR += 1;
		else gameEngine.teamB += 1;

		gameEngine.updateTeamReady();
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
		buffer.write<int>(gameEngine.id_name.size());
		for (auto it = gameEngine.id_name.begin(); it != gameEngine.id_name.end(); it++) {
			buffer.write<int>(it->first);
			buffer.write<std::string>(it->second);
		}
		c->send(buffer);
		c->on(NetMessage::NAME, addPlayerName);
		c->on(NetMessage::TEAM, handleTeamSelection);
		c->on(NetMessage::READY, [&] (Connection *c, NetBuffer &buffer){
			bool ready2go = buffer.read < bool >();
			if (ready2go) gameEngine.readyPlayers += 1;
			else gameEngine.readyPlayers -= 1;
			gameEngine.updateTeamReady();
		});

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

		gameEngine.createPlayer(c);
    
		auto ps = new ParticleEmitter();
		ps->setGravity(-15.0f);
		ps->setCreationSpeed(500);
		ps->setInitialVel(vec3(0, 10, 0));
		ps->setTexture("Textures/gary.png");

		gameEngine.syncGameText(c);

		// Receive player keyboard and mouse(TODO) input
		c->on(NetMessage::PLAYER_INPUT, handlePlayerInput);
		c->on(NetMessage::READY_TOGGLE, handlePlayerReady);
		c->onDisconnected([&](Connection *c) {
			gameEngine.onPlayerDisconnected(c);
			std::cout << "Player " << c->getId() << " has disconnected."
				<< std::endl;
		});

		c->on(
			NetMessage::DEBUG_MAP,
			[&](Connection *c, NetBuffer &buf) {
				gameEngine.cleanMap();
			}
		);
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
