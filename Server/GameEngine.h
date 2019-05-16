#pragma once

#include <Shared/CommonStructs.h>
#include <Shared/GameObject.h>
#include <Shared/Player.h>
#include <Shared/Ball.h>
#include <Shared/Wall.h>
#include <Shared/GameState.h>
#include <Shared/Networking/Connection.h>

#define NUM_PLAYERS 4
#define MOVEMENT_MASK 0b11111
#define COMMAND_MASK 0b1100000

class GameEngine {
public:
	void init();

	void onPlayerDisconnected(Connection *c);

	void updateGameState(vector<PlayerInputs> & playerInputs);
	void synchronizeGameState();
	GameState & getGameState();
	
	template<class T>
	T* addGameObject() {
		auto id = gameState.getFreeId();
		auto obj = new T(id);
		addGameObject(obj);
		return obj;
	}

	void addGameObject(Player *player);
	void addGameObject(Ball *ball);
	void addGameObject(Wall *wall);
	void removeGameObjectById(int id);

	/* Functions for handling the MenuOptions */
	MenuOptions getTeams();//returns the serverMenuOptions currently (since the menu options are ony about teams right now)
	bool updateMenuOptions(MenuOptions playerMenuOptions);//will return true if an update was legal, false if illegal
	int nextAvailableSpot(int clientID);//will return the next available spot in the game (default team picking) 
	int getTeamOf(int clientID);//will return the team that the player belongs to (-1 if not assigned to a team)

	vec3 movementInputToVector(int movementInput);
	void movePlayers(vector<PlayerInputs> & playerInputs);
	void moveBalls();
	void doPlayerCommands(vector<PlayerInputs> & playerInputs);

	void doCollisionInteractions();
	void removeDeadObjects();
	void updateGameObjectsOnServerTick();
	bool noCollisionMove(GameObject * gameObject, vec3 movement);

	const vector<GameObject*> &getGameObjects() const;
	int getTeam();

private:
	int num_players = 0;
	MenuOptions serverMenuOptions;
	GameState gameState;
	void addGenericGameObject(GameObject *player);
};