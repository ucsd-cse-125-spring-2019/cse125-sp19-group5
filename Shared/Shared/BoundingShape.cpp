#include "BoundingShape.h"

BoundingShape::BoundingShape(vec3 position) {
	this->position = position;
}


vec3 BoundingShape::getPosition() {
	return this->position;
}

void BoundingShape::setPosition(vec3 position) {
this->position = position;
}