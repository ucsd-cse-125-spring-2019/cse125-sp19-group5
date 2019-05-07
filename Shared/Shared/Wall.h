#pragma once
#include "GameObject.h"

struct Plane;

class Wall : public GameObject {
public:
	Wall(vec3 position, vec3 velocity, int id, float length, float width, float height);
	int getGameObjectType() const;

private:
	float length;
	float width;
	float height;
	vector<Plane> facePlanes;
};

struct Plane {
	Plane(vec3 A, vec3 B, vec3 C) {
		// ax + by + cz + d = 0;
		vec3 abc = glm::cross((B - A), (C - A));
		this->a = abc.x;
		this->b = abc.y;
		this->c = abc.z;
		this->d = -(a*A.x + b * A.y + c * A.z);
	}
	float a;
	float b;
	float c;
	float d;

	string to_string() {
		return "a: " + std::to_string(a) + 
			", b: " + std::to_string(b) + 
			", c: " + std::to_string(c )+ 
			", d: " + std::to_string(d);
	}
};