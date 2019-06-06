#include <Shared/Player.h>
#include "Game/Powerups/SpeedBoost.h"

void Player::addPowerup(POWERUP_TYPES powerup) {
	switch (powerup) {
		case POWERUP_SPEEDBOOST: {
			addPowerup<SpeedBoost>();
			break;
		}
	}
}