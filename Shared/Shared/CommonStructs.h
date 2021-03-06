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
	SWING = 1 << 6,
	SHOOT = 1 << 7
};

struct PlayerInputs {
	vec3 direction;
	int inputs;
	int id;
};

