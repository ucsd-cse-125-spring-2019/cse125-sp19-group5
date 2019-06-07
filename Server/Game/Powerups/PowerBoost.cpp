#include "PowerBoost.h"

const POWERUP_TYPES PowerBoost::TYPE = POWERUP_POWERBOOST;

float PowerBoost::getDuration() const {
	return 10.0f;
}

void PowerBoost::onActivate() {
	player.setStrength(player.getStrength() * 2);
}

void PowerBoost::onDeactivate() {
	player.setStrength(player.getStrength() / 2);
}