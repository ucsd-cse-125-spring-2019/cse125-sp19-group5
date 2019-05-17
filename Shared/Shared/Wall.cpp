#include "Wall.h"
#include "BoundingBox.h"

Wall::Wall(vec3 position, vec3 direction, int id, float length, float width, float height) : GameObject(position, vec3(0.0f), id) {
	this->direction = direction;
	this->length = length;
	this->width = width;
	this->height = height;

	setBoundingShape(new BoundingBox(position, direction, length, width, height));
}

GAMEOBJECT_TYPES Wall::getGameObjectType() const {
	return WALL_TYPE;
}

void Wall::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Wall::onCollision(Ball * ball) { }

void Wall::onCollision(Paddle * paddle) { }

void Wall::onCollision(Player * player) { }

void Wall::onCollision(Wall * wall) { }