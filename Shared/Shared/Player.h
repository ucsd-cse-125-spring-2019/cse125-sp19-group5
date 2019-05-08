#pragma once
#include "GameObject.h"
#include "Wall.h"
#include "CommonStructs.h"
#include "Paddle.h"

class Player : public GameObject {
public:
	Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team);
	int getGameObjectType() const;
	void onCollision(GameObject * gameObject);
	vec3 getMoveDestination(vec3 movement);
	vec3 getDirection();
	GameObject * doAction(PlayerCommands action);
	GameObject * processCommand(int inputs);
private:
	vec3 direction;
	float radius;
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;
};