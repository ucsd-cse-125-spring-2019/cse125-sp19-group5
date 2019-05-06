#include "Wall.h"

Wall::Wall(vec3 position, vec3 velocity, int id, float length, float width, float height) : GameObject(id) {
	this->length = length;
	this->width = width;
	this->height = height;
}

int Wall::getGameObjectType() const {
	return WALL_TYPE;
}