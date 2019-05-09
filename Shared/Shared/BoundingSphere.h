#pragma once
#include "BoundingShape.h"

class BoundingSphere : public BoundingShape {
public:
	Plane * bottom;
	Plane * top;
	Plane * front;
	Plane * back;
	Plane * left;
	Plane * right;
	BoundingSphere(vec3 position, float radius);
	float getRadius();
	void setRadius(float radius);

	// collision stuff below
	bool collideVisit(BoundingShape * boundingShape);
	bool collidesWith(BoundingBox * boundingBox);
	bool collidesWith(BoundingSphere * boundingSphere);

protected:
	float radius;
};