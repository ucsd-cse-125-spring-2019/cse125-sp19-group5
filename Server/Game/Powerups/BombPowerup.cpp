#include "BombPowerup.h"

const POWERUP_TYPES BombPowerup::TYPE = POWERUP_BOMBS;

float BombPowerup::getDuration() const {
	return -1.0f;
}

void BombPowerup::onActivate() {
}

void BombPowerup::onDeactivate() {
}