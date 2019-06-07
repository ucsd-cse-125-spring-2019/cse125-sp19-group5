#include <Shared/Player.h>
#include <Shared/PowerUpItem.h>
#include "Game/Powerups/SpeedBoost.h"
#include "Game/Powerups/PowerBoost.h"
#include "Game/Powerups/BulletSpray.h"
#include "Game/Powerups/StunGun.h"
#include "Game/Powerups/StunDebuff.h"
#include "Game/Powerups/LowerCooldowns.h"
#include "Game/Powerups/BombPowerup.h"

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
		case POWERUP_BULLET_SPRAY: {
			addPowerup<BulletSpray>();
			break;
		} 
		case POWERUP_STUNGUN: {
			addPowerup<StunGun>();
			break;
		}
		case POWERUP_LOWER_COOLDOWNS: {
			addPowerup<LowerCooldowns>();
			break;
		}
		case POWERUP_BOMBS: {
			addPowerup<BombPowerup>();
			break;
		}
		default:
			break;
	}
}

void Player::onCollision(StunBullet * stunBullet) {
	addPowerup<StunDebuff>();
}