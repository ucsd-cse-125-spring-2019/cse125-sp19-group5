#include "GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "Wall.h"

struct GameState : public Serializable {
	vector<Player *> players;
	vector<Ball *> balls;
	vector<Wall *> walls;
	vector<GameObject *> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;

	GameState() : gameObjects(1024, nullptr) { }

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
