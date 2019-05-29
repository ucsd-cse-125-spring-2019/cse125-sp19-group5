#include "Bullet.h"

Bullet::Bullet(vec3 position, vec3 velocity, float radius) : SphereGameObject(position, velocity, 0) {
	setBoundingShape(new BoundingSphere(position, radius));
}

GAMEOBJECT_TYPES Bullet::getGameObjectType() const {
	return BULLET_TYPE;
}

void Bullet::updateOnServerTick() {
	move(getVelocity());
}

bool Bullet::deleteOnServerTick() {
	return this->hit;
}

void Bullet::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Bullet::onCollision(Ball * ball) {
	this->hit = true;
}

void Bullet::onCollision(Wall * wall) {
	this->hit = true;
}