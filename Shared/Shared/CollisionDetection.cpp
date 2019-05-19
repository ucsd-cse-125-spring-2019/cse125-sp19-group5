#include "CollisionDetection.h"
#include <algorithm>

bool CollisionDetection::collisionCheck(BoundingBox * box1, BoundingBox * box2) {
	// TODO
	if (box1 == box2) {
		return false;
	}
	return false;
}


bool CollisionDetection::collisionCheck(BoundingSphere * sphere1, BoundingSphere * sphere2) {
	if (sphere1 == sphere2) {
		return false;
	}
	return glm::distance(sphere1->getPosition(), sphere2->getPosition()) <= (sphere1->getRadius() + sphere2->getRadius());
}

bool CollisionDetection::sphereInsidePlane(BoundingSphere * sphere, Plane * p) {
	return -p->pointDistance(sphere->getPosition()) > sphere->getRadius();
}

bool CollisionDetection::sphereInsideBox(BoundingSphere * sphere, BoundingBox * box) {
	if (!sphereInsidePlane(sphere, box->front)) { return false; }
	if (!sphereInsidePlane(sphere, box->back)) { return false; }
	if (!sphereInsidePlane(sphere, box->top)) { return false; }
	if (!sphereInsidePlane(sphere, box->bottom)) { return false; }
	if (!sphereInsidePlane(sphere, box->left)) { return false; }
	if (!sphereInsidePlane(sphere, box->right)) { return false; }

	return true;
}

bool CollisionDetection::sphereIntersectsPlanePoint(BoundingSphere * sphere, Plane * p, vec3 * point, float * radius) {
	float d = p->pointDistance(sphere->getPosition());
	vec3 proj = p->getNormal() * d;
	*point = sphere->getPosition() - proj;
	*radius = sqrtf(std::max(sphere->getRadius() * sphere->getRadius() - d * d, 0.0f));
	return fabs(d) <= sphere->getRadius();
}

vector<Plane *> CollisionDetection::getIntersectingPlanes(BoundingSphere * sphere, BoundingBox * box) {
	vector<Plane *> intersectingPlanes;

	vec3 point;
	float radius;

	if (sphereIntersectsPlanePoint(sphere, box->top, &point, &radius)) {
		if (box->left->pointDistance(point) <= radius &&
			box->right->pointDistance(point) <= radius &&
			box->front->pointDistance(point) <= radius &&
			box->back->pointDistance(point) <= radius) {
			intersectingPlanes.push_back(box->top);
		}
	}

	if (sphereIntersectsPlanePoint(sphere, box->bottom, &point, &radius)) {
		if (box->left->pointDistance(point) <= radius &&
			box->right->pointDistance(point) <= radius &&
			box->front->pointDistance(point) <= radius &&
			box->back->pointDistance(point) <= radius) {
			intersectingPlanes.push_back(box->bottom);
		}
	}

	if (sphereIntersectsPlanePoint(sphere, box->left, &point, &radius)) {
		if (box->top->pointDistance(point) <= radius &&
			box->bottom->pointDistance(point) <= radius &&
			box->front->pointDistance(point) <= radius &&
			box->back->pointDistance(point) <= radius) {
			intersectingPlanes.push_back(box->left);
		}
	}

	if (sphereIntersectsPlanePoint(sphere, box->right, &point, &radius)) {
		if (box->top->pointDistance(point) <= radius &&
			box->bottom->pointDistance(point) <= radius &&
			box->front->pointDistance(point) <= radius &&
			box->back->pointDistance(point) <= radius) {
			intersectingPlanes.push_back(box->right);
		}
	}

	if (sphereIntersectsPlanePoint(sphere, box->front, &point, &radius)) {
		if (box->top->pointDistance(point) <= radius &&
			box->bottom->pointDistance(point) <= radius &&
			box->left->pointDistance(point) <= radius &&
			box->right->pointDistance(point) <= radius) {
			intersectingPlanes.push_back(box->front);
		}
	}

	if (sphereIntersectsPlanePoint(sphere, box->back, &point, &radius)) {
		if (box->top->pointDistance(point) <= radius &&
			box->bottom->pointDistance(point) <= radius &&
			box->left->pointDistance(point) <= radius &&
			box->right->pointDistance(point) <= radius) {
			intersectingPlanes.push_back(box->back);
		}
	}

	return intersectingPlanes;
}

bool CollisionDetection::sphereIntersectsBox(BoundingSphere * sphere, BoundingBox * box) {
	return getIntersectingPlanes(sphere, box).size() > 0;
}

bool CollisionDetection::collisionCheck(BoundingBox * box, BoundingSphere * sphere) {
	return sphereInsideBox(sphere, box) || sphereIntersectsBox(sphere, box);
}

bool CollisionDetection::collisionCheck(BoundingSphere * sphere, BoundingBox * box) {
	return collisionCheck(box, sphere);
}