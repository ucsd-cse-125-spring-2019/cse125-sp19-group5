#include "Bullet.h"
#include "CollisionDetection.h"
#include "Wall.h"

Bullet::Bullet(vec3 position, vec3 velocity, float radius) : SphereGameObject(position, velocity, 0) {
	setBoundingShape(new BoundingSphere(position, radius));
}

GAMEOBJECT_TYPES Bullet::getGameObjectType() const {
	return BULLET_TYPE;
}

void Bullet::updateOnServerTick() {
	move(getVelocity());
	lifespan--;
}

bool Bullet::deleteOnServerTick() {
	return this->hit || lifespan == 0;
}

void Bullet::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Bullet::onCollision(Ball * ball) {
	this->hit = true;
}

void Bullet::onCollision(Player * player) {
	this->hit = true;
}

void Bullet::onCollision(Wall * wall) {
	for (Plane * p : CollisionDetection::getIntersectingPlanes(this->getBoundingSphere(), wall->getBoundingBox())) {
		if (p != wall->getBoundingBox()->top && p != wall->getBoundingBox()->bottom) {
			this->hit = true;
		}
	}
}