#include "BombPowerup.h"

const POWERUP_TYPES BombPowerup::TYPE = POWERUP_BOMBS;

float BombPowerup::getDuration() const {
	return 20.0f;
}

void BombPowerup::onActivate() {
}

void BombPowerup::onDeactivate() {
}