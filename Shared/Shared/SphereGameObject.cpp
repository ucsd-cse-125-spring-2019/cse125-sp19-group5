#include "SphereGameObject.h"

SphereGameObject::SphereGameObject(vec3 position, vec3 velocity, int id, float radius) : GameObject(position, vec3(0.0f), id) {
	setBoundingShape(new BoundingSphere(position, radius));
}

void SphereGameObject::setBoundingShape(BoundingSphere * sphere) {
	this->boundingSphere = sphere;
	this->boundingShape = sphere;
}

BoundingSphere * SphereGameObject::getBoundingSphere() {
	return this->boundingSphere;
}