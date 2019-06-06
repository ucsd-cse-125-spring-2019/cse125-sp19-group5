#include "Wall.h"

GAMEOBJECT_TYPES Wall::getGameObjectType() const {
	return WALL_TYPE;
}

void Wall::updateOnServerTick() {
	if (destroyable) {
		move(vec3(0, 0.1, 0));
	}
	if (getPosition().y > 0) {
		setPosition(vec3(getPosition().x, 0, getPosition().z));
	}
}

bool Wall::deleteOnServerTick() {
	return this->health <= 0;
}

void Wall::setDestroyable(bool destroyable) {
	this->destroyable = destroyable;
}

void Wall::setHealth(int health) {
	this->health = health;
}


void Wall::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Wall::onCollision(Ball * ball) { }

void Wall::onCollision(Bullet * bullet) {
	if (this->destroyable) {
		this->health -= 1;
	}
}

void Wall::onCollision(Paddle * paddle) { 
	if (this->destroyable) {
		this->health -= 1;
	}
}

void Wall::onCollision(Player * player) { }

void Wall::onCollision(Wall * wall) { }

void Wall::onCreated() {
}
