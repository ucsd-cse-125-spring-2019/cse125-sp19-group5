#pragma once
#include "Common.h"

// declare classes to avoid circular dependencies
class BoundingBox;
class BoundingSphere;

class BoundingShape {
public:
	BoundingShape(vec3 position);
	vec3 getPosition();
	void setPosition(vec3 position);

	// collision stuff below
	virtual bool collideVisit(BoundingShape * boundingShape) = 0;
	virtual bool collidesWith(BoundingBox * boundingBox) = 0;
	virtual bool collidesWith(BoundingSphere * boundingSphere) = 0;

protected:
	vec3 position;
};