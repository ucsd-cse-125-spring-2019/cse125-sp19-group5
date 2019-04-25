#pragma once
#include "Common.h"

enum PlayerCommands {
	FORWARD = 1 << 0,
	BACKWARD = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	JUMP = 1 << 4,
	WALL = 1 << 5,
	SWING = 1 << 6
};

class GameObject;
class Player;
class Ball;
class Wall;

struct GameState {
	vector<Player *> players;
	vector<Ball *> balls;
	vector<Wall *> walls;
	vector<GameObject *> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;
};

struct NetworkGameState {
	vector<GameObject> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;
};

struct PlayerInputs {
	int inputs;
	int id;
};