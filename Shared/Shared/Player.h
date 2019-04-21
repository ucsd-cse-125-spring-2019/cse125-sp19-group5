#pragma once
#include "GameObject.h"
#include "Wall.h"
#include "CommonStructs.h"

class Player : public GameObject {
public:
	Player(vec3 position, vec3 velocity, vec3 direction, string id, int radius);
	void onCollision(GameObject &gameObject);
	vec3 getDirection();
	void doAction(PlayerCommands action);
	void processCommand(int inputs);
private:
	int team;
	int actionCharge;
	PlayerCommands currentAction;
	vector<Wall *> walls;
	vec3 direction;
};
