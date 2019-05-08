#include "GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "Wall.h"

struct GameState {
	vector<Player *> players;
	vector<Ball *> balls;
	vector<Wall *> walls;
	vector<GameObject *> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;
};

struct GameStateNet : public Serializable {
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
			GameObject gameObject(vec3(0), vec3(0), 0);
			gameObject.deserialize(buffer);
			gameObjects.push_back(gameObject);
		}

		timeLeft = buffer.read<long>();
		score = buffer.read<tuple<int, int>>();
		in_progress = buffer.read<bool>();
	}
};