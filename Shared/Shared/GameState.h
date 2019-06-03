#pragma once

#include "GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "Wall.h"
#include <array>

// Maximum number of players in the game.
constexpr auto MAX_PLAYERS = 4;

struct GameState : public Serializable {
	vector<Player *> players;
	vector<Ball *> balls;
	vector<Goal *> goals;
	vector<Wall *> walls;
	std::array<GameObject *, MAX_GAME_OBJS> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;

	GameState() : gameObjects({ nullptr }) {}

	int getFreeId() const {
		// Offset by MAX_PLAYERS to reserve IDs for players.
		for (int id = MAX_PLAYERS; id < gameObjects.size(); id++) {
			if (gameObjects[id] == nullptr) {
				return id;
			}
		}
		return -1;
	}

	void serialize(NetBuffer &buffer) const {
		auto size = 0;
		for (auto gameObject : gameObjects) {
			if (gameObject) {
				size++;
			}
		}
		buffer.write<size_t>(size);
		for (auto gameObject : gameObjects) {
			if (!gameObject) {
				continue;
			}
			buffer.write(gameObject->getId());
			gameObject->serialize(buffer);
		}

		buffer.write<long>(timeLeft);
		buffer.write<tuple<int, int>>(score);
		buffer.write<bool>(in_progress);
	}

	void deserialize(NetBuffer &buffer) {
		constexpr auto origin = vec3(0.0f);

		auto numGameObjects = buffer.read<size_t>();
		for (int i = 0; i < numGameObjects; i++) {
			auto gameObject = gameObjects[buffer.read<int>()];
			if (gameObject) {
				gameObject->deserialize(buffer);
			}
		}

		timeLeft = buffer.read<long>();
		score = buffer.read<tuple<int, int>>();
		in_progress = buffer.read<bool>();
	}
};
