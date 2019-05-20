#pragma once
#include "SphereGameObject.h"
#include "Goal.h"
#include "Paddle.h"
#include "Player.h"
#include "Wall.h"

class Ball : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;
	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();
	bool getGoalScored();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Goal * goal);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	bool goalScored = false;
};