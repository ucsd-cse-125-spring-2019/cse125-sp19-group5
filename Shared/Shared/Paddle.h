#pragma once
#include <set>
#include "SphereGameObject.h"

class Paddle : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;

	Paddle(vec3 position, vec3 velocity, int id, float radius, int lifespan);
	GAMEOBJECT_TYPES getGameObjectType() const;
	bool deleteOnServerTick();
	void updateOnServerTick();
	std::set<GameObject *> & getObjectsHit();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

	void setLifespan(int newLifespan);
	int getLifespan() const;

	void setOwner(Player * owner);
	Player * getOwner();

	void onCreated() override;

private:
	int lifespan;
	std::set<GameObject *> objectsHit;
	Player * owner = nullptr;
};