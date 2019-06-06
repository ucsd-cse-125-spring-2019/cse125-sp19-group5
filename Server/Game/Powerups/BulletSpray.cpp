#include "BulletSpray.h"

const POWERUP_TYPES BulletSpray::TYPE = POWERUP_BULLET_SPRAY;

float BulletSpray::getDuration() const {
	return 20.0f;
}

void BulletSpray::onActivate() {
	player.setNumBullets(5);
}

void BulletSpray::onDeactivate() {
	player.setNumBullets(1);
}