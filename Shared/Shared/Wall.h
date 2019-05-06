#pragma once
#include "GameObject.h"

class Wall : public GameObject {
public:
	Wall(vec3 position, vec3 velocity, int id, float length, float width, float height);
	int getGameObjectType() const;

private:
	float length;
	float width;
	float height;
};