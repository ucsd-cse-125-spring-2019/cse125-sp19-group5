#include "LowerCooldowns.h"

const POWERUP_TYPES LowerCooldowns::TYPE = POWERUP_LOWER_COOLDOWNS;

float LowerCooldowns::getDuration() const {
	return 20.0f;
}

void LowerCooldowns::onActivate() {
	player.setCooldown(SWING, std::make_tuple(0, 15));
	player.setCooldown(SHOOT, std::make_tuple(0, 15));
	player.setCooldown(WALL, std::make_tuple(0, 60));
}

void LowerCooldowns::onDeactivate() {
	player.setCooldown(SWING, std::make_tuple(0, 30));
	player.setCooldown(SHOOT, std::make_tuple(0, 30));
	player.setCooldown(WALL, std::make_tuple(0, 120));
}