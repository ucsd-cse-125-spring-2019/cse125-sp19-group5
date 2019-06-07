#pragma once
#include "SphereGameObject.h"

class Bomb : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;

	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();
	bool deleteOnServerTick();

	bool getHit();
	void setOwner(Player * p);
	Player * getOwner();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	bool hit = false;
	int lifespan = 120;
	Player * owner;
};