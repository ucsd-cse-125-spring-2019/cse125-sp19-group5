#pragma once
#include "GameObject.h"

class Player : public GameObject {
private:
	int team;
	int swingCooldown;
	vector<Wall *> walls;
};
