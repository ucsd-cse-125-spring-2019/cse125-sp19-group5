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

	//TODO: config file for the port number to be specified in
	Network::init(1234);

	GameEngine gameEngine;
	gameEngine.init();

	auto origin = vec3(0.0f);

	vector<PlayerInputs> playerInputs;

	auto ground = gameEngine.addGameObject<Wall>();
	ground->setBoundingShape(new BoundingBox(vec3(0, 0, 0), vec3(1, 0, 0), 1, 1, 100));
	ground->setModel("Models/ground.obj");
	ground->setMaterial("Materials/grass.json");

	auto ball = gameEngine.addGameObject<Ball>();
	ball->setBoundingShape(new BoundingSphere(origin, 4));
	ball->setScale(vec3(1.0f/4.0f));
	ball->setPosition(vec3(0, 4, 0));
	ball->setVelocity(vec3(0, 0, 0));
	ball->setModel("Models/sphere.obj");
	ball->setMaterial("Materials/brick.json");


	// Handle player keyboard/mouse inputs
	auto handlePlayerInput = [&playerInputs](Connection *c, NetBuffer &buffer) {
		PlayerInputs input;
		input.id = c->getId();
		input.inputs = buffer.read<int>();
		input.direction = buffer.read<vec3>();
		playerInputs.push_back(input);
	};

	//handle menu option selections made by the client 
	auto handleMenuInput = [&](Connection *c, NetBuffer &playerMenuInput) 
	{
		MenuOptions playerMenuOptions = playerMenuInput.read<MenuOptions>();

		//check if the update is allowed
		if (gameEngine.updateMenuOptions(playerMenuOptions)) 
		{
			//the update was accepted, broadcast the confirmation to all clients
			NetBuffer teamUpdate(NetMessage::MENU_CONFIRM);
			teamUpdate.write<MenuOptions>(gameEngine.getTeams());
			Network::broadcast(teamUpdate);

			cout << "broadcast complete" << endl;

			//create a new player
			auto player = new Player(origin, origin, origin, c->getId(), 1.0f, gameEngine.getTeamOf(c->getId()));
			player->setDirection(vec3(0, 0, -1));
			player->setScale(vec3(0.2f));
			player->setModel("Models/player.obj");
			player->setDirection(vec3(0, 0, -1));
			player->setMaterial("Materials/brick.json");

			//add this player to the game
			gameEngine.addGameObject(player);

			//listen for player input at this point
			c->on(NetMessage::PLAYER_INPUT, handlePlayerInput);
			cout << "listening to player input" << endl;
		}
		else 
		{
			//the update was rejected, notify the client affected
			NetBuffer pickAgain(NetMessage::MENU_INPUT);
			pickAgain.write<MenuOptions>(gameEngine.getTeams());
			c->send(pickAgain);
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
		menu_options.write<MenuOptions>(gameEngine.getTeams());
		c->send(menu_options);
		c->on(NetMessage::MENU_INPUT, handleMenuInput);//callback for the client reply
		/*DEBUG*/
		cout << "send menu options" << endl;
		/*END_DEBUG*/

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

		// Receive player keyboard and mouse(TODO) input
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