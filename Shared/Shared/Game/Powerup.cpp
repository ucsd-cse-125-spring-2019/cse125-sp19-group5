#include "Powerup.h"
#include "../Util/CurTime.h"

Powerup::Powerup(Player &player) : player(player) { }

void Powerup::activate() {
	auto duration = this->getDuration();
	if (duration < 0.0f) {
		deactivateTime = -1.0f;
	} else {
		deactivateTime = curTime() + duration;
	}
}

float Powerup::getDuration() const {
	return -1.0f;
}

bool Powerup::shouldDeactivate() const {
	if (deactivateTime < 0.0f) { return false; }
	return curTime() >= deactivateTime;
}

void Powerup::onActivate() { }

void Powerup::onDeactivate() { }

void Powerup::update() { }
