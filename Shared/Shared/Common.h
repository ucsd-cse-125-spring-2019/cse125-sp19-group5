#pragma once

// Commonly used imports and using statements.

#include <glm/glm.hpp>
#include <vector>
#include <tuple>


using std::vector;
using std::tuple;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

enum PlayerCommands {
	FORWARD = 1 << 0,
	BACKWARD = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	JUMP = 1 << 4,
	WALL = 1 << 5,
	SWING = 1 << 6
};

struct Player {
	GameObjectState state;
	int id;
	int team;
	int swingCooldown;
};

struct Ball {
	GameObjectState state;
	int id;
};

struct Wall {
	GameObjectState state;
	int id;
};

struct GameState {
	vector<Player> players;
	vector<Ball> balls;
	vector<Wall> walls;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;
};

struct PlayerInputs {
	int inputs;
	int id;
};