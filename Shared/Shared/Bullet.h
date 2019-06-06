#pragma once
#include "SphereGameObject.h"

class Bullet : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;

	Bullet(vec3 position, vec3 velocity, float radius);
	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();
	bool deleteOnServerTick();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	bool hit = false;
	int lifespan = 100;
};