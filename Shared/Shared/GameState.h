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

	GameState() : gameObjects(1024, nullptr) { }
};

struct GameStateNet : public Serializable {
	vector<GameObject*> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;

	GameStateNet(): gameObjects(1024, nullptr) { }

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
			buffer.write<GAMEOBJECT_TYPES>(gameObject->getGameObjectType());
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
			GameObject *gameObject;
			switch (buffer.read<GAMEOBJECT_TYPES>()) {
				case GAMEOBJECT_TYPES::BALL_TYPE:
					gameObject = new Ball(origin, origin, -1, 0);
					break;
				case GAMEOBJECT_TYPES::PADDLE_TYPE:
					gameObject = new Paddle(origin, origin, -1, 0, 0);
				case GAMEOBJECT_TYPES::PLAYER_TYPE:
					gameObject = new Player(-1);
					break;
				case GAMEOBJECT_TYPES::WALL_TYPE:
					gameObject = new Wall(origin, origin, -1, 0);
					break;
				default:
					gameObject = nullptr;
					break;
			}

			if (!gameObject) { continue; }

			gameObject->deserialize(buffer);

			gameObjects[gameObject->getId()] = gameObject;
		}

		timeLeft = buffer.read<long>();
		score = buffer.read<tuple<int, int>>();
		in_progress = buffer.read<bool>();
	}
};