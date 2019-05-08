#pragma once
#include "GameObject.h"

struct Plane;

class Wall : public GameObject {
public:
	Wall(vec3 position, vec3 direction, int id, float length, float width, float height);
	int getGameObjectType() const;

private:
	vec3 direction;
	float length;
	float width;
	float height;
	vector<Plane> facePlanes;
};