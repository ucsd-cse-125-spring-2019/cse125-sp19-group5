#include "BoxGameObject.h"

BoxGameObject::BoxGameObject(vec3 position, vec3 direction, int id, float width, float height, float length) : GameObject(position, vec3(0.0f), id) {
	this->direction = direction;
	setBoundingShape(new BoundingBox(position, direction, width, length, height));
}

vec3 BoxGameObject::setPosition(vec3 position) {
	vec3 ret = GameObject::setPosition(position);
	float width = getBoundingBox()->width;
	float height = getBoundingBox()->height;
	float length = getBoundingBox()->length;
	vec3 dir = getBoundingBox()->direction;
	setBoundingShape(new BoundingBox(getPosition(), dir, width, height, length));
	return ret;
}

void BoxGameObject::setBoundingShape(BoundingBox * boundingBox) {
	this->boundingShape = boundingBox;
	this->boundingBox = boundingBox;
}

BoundingBox * BoxGameObject::getBoundingBox() {
	return this->boundingBox;
}