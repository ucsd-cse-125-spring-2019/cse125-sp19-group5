#include "PowerUpItem.h"
#include "Wall.h"

GAMEOBJECT_TYPES PowerUpItem::getGameObjectType() const {
	return POWERUP_ITEM_TYPE;
}

void PowerUpItem::updateOnServerTick() {
	if (!grounded) {
		move(vec3(0, -0.5, 0));
	}
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
	setPosition(vec3(
		getPosition().x,
		wall->getPosition().y + wall->getBoundingBox()->height + getBoundingSphere()->getRadius(),
		getPosition().z
	));
	grounded = true;
}