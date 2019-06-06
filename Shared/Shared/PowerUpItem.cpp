#include "PowerUpItem.h"

GAMEOBJECT_TYPES PowerUpItem::getGameObjectType() const {
	return POWERUP_ITEM_TYPE;
}

void PowerUpItem::updateOnServerTick() {
}

bool PowerUpItem::deleteOnServerTick() {
	return this->pickedUp;
}

void PowerUpItem::setPowerUpType(POWERUP_TYPES powerUpType) {
	this->powerUpType = powerUpType;
}
POWERUP_TYPES PowerUpItem::getPowerUpType() {
	return this->powerUpType;
}

void PowerUpItem::onCollision(GameObject * gameObject) {
	return gameObject->onCollision(this);
}

void PowerUpItem::onCollision(Player * player) {
	this->pickedUp = true;
}

void PowerUpItem::onCollision(Wall * wall) {
}