#include <Shared/Player.h>
#include "Game/Powerups/SpeedBoost.h"

void Player::onCollision(PowerUpItem * item) {
	switch (item->getPowerUpType()) {
		case POWERUP_SPEEDBOOST: {
			addPowerup<SpeedBoost>();
			break;
		}
	}
}