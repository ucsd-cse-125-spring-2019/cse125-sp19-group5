#pragma once
#include "SphereGameObject.h"

class PowerUpItem : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;

	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();
	bool deleteOnServerTick();

	void setPowerUpType(POWERUP_TYPES powerUpType);
	POWERUP_TYPES getPowerUpType();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	bool pickedUp = false;
	POWERUP_TYPES powerUpType;
};