#pragma once
#include "Bullet.h"

class StunBullet : public Bullet {
public:
	using Bullet::Bullet;

	GAMEOBJECT_TYPES getGameObjectType() const;
	void onCollision(GameObject * gameObject);
};