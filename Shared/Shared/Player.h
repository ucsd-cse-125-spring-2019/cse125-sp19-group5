#pragma once
#include "GameObject.h"
#include "Wall.h"
#include "CommonStructs.h"
#include "Paddle.h"

class Player : public GameObject {
public:
	using GameObject::GameObject;
  
  Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team);

	void setDirection(const vec3 &newDirection);
	GAMEOBJECT_TYPES getGameObjectType() const;
	void onCollision(GameObject * gameObject);
	vec3 getMoveDestination(vec3 movement);
	vec3 getDirection();
	GameObject * doAction(PlayerCommands action);
	GameObject * processCommand(int inputs);

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;
private:
	vec3 direction;
	float radius;
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;
};