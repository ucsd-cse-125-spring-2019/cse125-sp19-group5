#include "StunBullet.h"

GAMEOBJECT_TYPES StunBullet::getGameObjectType() const {
	return STUN_BULLET_TYPE;
}
void StunBullet::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}