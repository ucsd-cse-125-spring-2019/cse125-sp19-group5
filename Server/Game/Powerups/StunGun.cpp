#include "StunGun.h"

const POWERUP_TYPES StunGun::TYPE = POWERUP_STUNGUN;

float StunGun::getDuration() const {
	return 10.0f;
}

void StunGun::onActivate() {
	player.setBulletType(BULLET_STUN);
}

void StunGun::onDeactivate() {
	player.setBulletType(BULLET_DEFAULT);
}