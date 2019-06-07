#include "StunDebuff.h"
#include <iostream>
#include <Shared/PhysicsEngine.h>

const POWERUP_TYPES StunDebuff::TYPE = POWERUP_STUN_DEBUFF;

float StunDebuff::getDuration() const {
	return -1.0f;
}

void StunDebuff::onActivate() {
	player.setMoveSpeed(0);
}

void StunDebuff::onDeactivate() {
	player.setMoveSpeed(PhysicsEngine::getPlayerDefaultMoveSpeed());
}
