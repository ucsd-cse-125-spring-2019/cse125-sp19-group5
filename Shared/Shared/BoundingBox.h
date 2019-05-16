#pragma once
#include "BoundingShape.h"

struct Plane;

class BoundingBox : public BoundingShape {
public:
	Plane * bottom;
	Plane * top;
	Plane * front;
	Plane * back;
	Plane * left;
	Plane * right;
	BoundingBox(vec3 position, vec3 direction, float length, float width, float height);
	vector<Plane *> & getFacePlanes();

	// collision stuff below
	bool collideVisit(BoundingShape * boundingShape);
	bool collidesWith(BoundingBox * boundingBox);
	bool collidesWith(BoundingSphere * boundingSphere);

protected:
	vec3 direction;
	float length;
	float width;
	float height;
	vector<Plane *> facePlanes;
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
			", c: " + std::to_string(c) +
			", d: " + std::to_string(d);
	}

	vec3 getNormal() {
		return vec3(a, b, c);
	}

	void invertNormal() {
		this->a = -1 * this->a;
		this->b = -1 * this->b;
		this->c = -1 * this->c;
		this->d = -1 * this->d;
	}

	float pointDistance(vec3 point) {
		return (a*point.x + b * point.y + c * point.z + d) / (glm::sqrt(a*a + b * b + c * c));
	}
};