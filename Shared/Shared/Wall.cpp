#include "Wall.h"

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

void Wall::onCreated() {
	setBoundingShape(new BoundingBox(position, direction, 1.0f, 2.0f, 2.0f));
	setModel("Models/wall.obj");
	setMaterial("Materials/brick.json");
}
