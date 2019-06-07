#include "Bomb.h"
#include "PhysicsEngine.h"

GAMEOBJECT_TYPES Bomb::getGameObjectType() const {
	return BOMB_TYPE;
}

bool Bomb::deleteOnServerTick() {
	return lifespan == 0;
}

bool Bomb::getHit() {
	return hit;
}

void Bomb::setOwner(Player * p) {
	this->owner = p;
}

Player * Bomb::getOwner() {
	return this->owner;
}

void Bomb::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Bomb::onCollision(Ball * ball) {
	this->hit = true;
}

void Bomb::onCollision(Player * player) {
	this->hit = true;
}

void Bomb::onCollision(Wall * wall) {
	this->hit = true;
}