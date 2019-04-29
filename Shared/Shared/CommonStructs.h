#pragma once
#include "Common.h"
#include "GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "Wall.h"
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

struct GameState{
	vector<Player *> players;
	vector<Ball *> balls;
	vector<Wall *> walls;
	vector<GameObject *> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;
};

struct GameStateNet : Serializable {
	vector<GameObject> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;

	void serialize(NetBuffer &buffer) const {
		buffer.write<size_t>(gameObjects.size());
		for (auto &gameObject : gameObjects) {
			gameObject.serialize(buffer);
		}

		buffer.write<long>(timeLeft);
		buffer.write<tuple<int, int>>(score);
		buffer.write<bool>(in_progress);
	}

	void deserialize(NetBuffer &buffer) {
		auto numGameObjects = buffer.read<size_t>();
		for (int i = 0; i < numGameObjects; i++) {
			GameObject gameObject(vec3(0), vec3(0), "", 0);
			gameObject.deserialize(buffer);
			gameObjects.push_back(gameObject);
		}

		timeLeft = buffer.read<long>();
		score = buffer.read<tuple<int, int>>();
		in_progress = buffer.read<bool>();
	}
};

struct PlayerInputs {
	int inputs;
	int id;
};

