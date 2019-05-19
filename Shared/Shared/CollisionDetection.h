#pragma once
#include "BoundingBox.h"
#include "BoundingSphere.h"

class CollisionDetection {
public:
	static bool collisionCheck(BoundingBox * box1, BoundingBox * box2);
	static bool collisionCheck(BoundingSphere * sphere1, BoundingSphere * sphere2);

	static bool sphereInsidePlane(BoundingSphere * sphere, Plane * p);
	static bool sphereInsideBox(BoundingSphere * sphere, BoundingBox * box);
	static bool sphereIntersectsPlanePoint(BoundingSphere * sphere, Plane * p, vec3 * point, float * radius);
	static vector<Plane *> getIntersectingPlanes(BoundingSphere * sphere, BoundingBox * box);
	static bool sphereIntersectsBox(BoundingSphere * sphere, BoundingBox * box);
	static bool collisionCheck(BoundingBox * box, BoundingSphere * sphere);
	static bool collisionCheck(BoundingSphere * sphere, BoundingBox * box);
};