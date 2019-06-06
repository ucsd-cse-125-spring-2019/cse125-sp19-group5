#include <Shared/Player.h>
#include "Game/Powerups/SpeedBoost.h"
#include "Game/Powerups/PowerBoost.h"

void Player::onCollision(PowerUpItem * item) {
	switch (item->getPowerUpType()) {
		case POWERUP_SPEEDBOOST: {
			addPowerup<SpeedBoost>();
			break;
		}
		case POWERUP_POWERBOOST: {
			addPowerup<PowerBoost>();
			break;
		}
		default:
			break;
	}
}