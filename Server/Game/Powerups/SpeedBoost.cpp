#include "SpeedBoost.h"
#include <iostream>
#include <Shared/PhysicsEngine.h>

const string SpeedBoost::TYPE = "speed";

float SpeedBoost::getDuration() const {
	return 10.0f;
}

void SpeedBoost::onActivate() {
	player.setMoveSpeed(PhysicsEngine::getPlayerDefaultMoveSpeed() * 2.0f);
}

void SpeedBoost::onDeactivate() {
	player.setMoveSpeed(PhysicsEngine::getPlayerDefaultMoveSpeed());
}
