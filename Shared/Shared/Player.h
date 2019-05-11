#pragma once
#include "GameObject.h"
#include "Wall.h"
#include "CommonStructs.h"
#include "Paddle.h"

class Player : public GameObject {
public:
	using GameObject::GameObject;

	Player(vec3 position, vec3 velocity, vec3 direction, int id, int radius);

	void setDirection(const vec3 &newDirection);

	GAMEOBJECT_TYPES getGameObjectType() const;
	void onCollision(GameObject * gameObject);
	vec3 getMoveDestination(vec3 movement);
	vec3 getDirection();
	int getTeam();
	bool setTeam(int team);
	GameObject * doAction(PlayerCommands action);
	GameObject * processCommand(int inputs);

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;
private:
	int team;
	int actionCharge;
	PlayerCommands currentAction;
	vector<Wall *> walls;
	vec3 direction;
};
