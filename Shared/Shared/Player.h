#pragma once
#include "GameObject.h"
#include "Wall.h"

class Player : public GameObject {
public:
	Player(vec3 position, vec3 velocity, vec3 direction, string id, int radius);
	void onCollision(GameObject &gameObject);
	vec3 getDirection();

private:
	int team;
	int swingCooldown;
	vector<Wall *> walls;
	vec3 direction;
};
