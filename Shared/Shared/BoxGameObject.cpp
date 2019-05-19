#include "BoxGameObject.h"

BoxGameObject::BoxGameObject(vec3 position, vec3 direction, int id, float width, float height, float length) : GameObject(position, vec3(0.0f), id) {
	this->direction = direction;
	this->length = length;
	this->width = width;
	this->height = height;

	setBoundingShape(new BoundingBox(position, direction, width, length, height));
}


void BoxGameObject::setBoundingShape(BoundingBox * boundingBox) {
	this->boundingShape = boundingBox;
	this->boundingBox = boundingBox;
}

BoundingBox * BoxGameObject::getBoundingBox() {
	return this->boundingBox;
}