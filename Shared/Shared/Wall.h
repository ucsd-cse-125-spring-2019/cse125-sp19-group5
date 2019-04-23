#pragma once
#include "GameObject.h"

class Wall : public GameObject {
public:
	Wall(vec3 position, vec3 velocity, string id, int radius);
};