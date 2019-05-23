#include "BoundingSphere.h"
#include "CollisionDetection.h"

BoundingSphere::BoundingSphere(vec3 position, float radius) : BoundingShape(position) {
	this->radius = radius;
}

float BoundingSphere::getRadius() {
	return this->radius;
}

void BoundingSphere::setRadius(float radius) {
	this->radius = radius;
}

bool BoundingSphere::collideVisit(BoundingShape * boundingShape) {
	return boundingShape->collidesWith(this);
}
bool BoundingSphere::collidesWith(BoundingBox * boundingBox) {
	return CollisionDetection::collisionCheck(this, boundingBox);
}
bool BoundingSphere::collidesWith(BoundingSphere * boundingSphere) {
	return CollisionDetection::collisionCheck(this, boundingSphere);
}