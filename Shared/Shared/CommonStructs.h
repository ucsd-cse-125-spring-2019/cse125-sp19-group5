#pragma once
#include "Common.h"
#include "Networking/Serializable.h"

enum PlayerCommands {
	FORWARD = 1 << 0,
	BACKWARD = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	JUMP = 1 << 4,
	WALL = 1 << 5,
	SWING = 1 << 6
};

struct PlayerInputs {
	vec3 direction;
	int inputs;
	int id;
};

/*
 * This will be sent between the client and server
 * before the game has even begun. This currently
 * only handles team selection and nothing else
 */
struct MenuOptions {
	//each int will hold the id of the player 
	//assigned to that team and that position
	int team_A_1 = -1;
	int team_A_2 = -1;

	int team_B_1 = -1;
	int team_B_2 = -1;
};

/*
 * This will be used to select the abilities a player
 * use during the game. TODO: implement the abilities
 */
struct LoadoutOptions {
	int player_id = -1;;
	//this is a list of the different abilities a player
	//can use during a game
	bool charged_swing = false;
	bool extra_wall = false;
	bool gun_equipped = false;
};
