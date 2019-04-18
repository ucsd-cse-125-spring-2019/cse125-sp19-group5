#pragma once
#include "GameObject.h"
#include "Wall.h"

class Player : public GameObject {
public:
	Player(vec3 position, vec3 velocity, string id, int radius);
	void onCollision(GameObject &gameObject);

private:
	int team;
	int swingCooldown;
	vector<Wall *> walls;
};
