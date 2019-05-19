#pragma once
#include "SphereGameObject.h"
#include "Ball.h"
#include "Player.h"
#include "Wall.h"

class Paddle : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;

	Paddle(vec3 position, vec3 velocity, int id, float radius, int lifespan);
	GAMEOBJECT_TYPES getGameObjectType() const;
	bool deleteOnServerTick();
	void updateOnServerTick();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	int lifespan;
};