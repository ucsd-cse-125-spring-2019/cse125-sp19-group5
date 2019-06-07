#pragma once
#include <set>
#include "SphereGameObject.h"

#define GROUNDED_TICKS_THRESHOLD 10

class Ball : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;
	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();
	bool getGoalScored();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Bullet * bullet);
	void onCollision(Goal * goal);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	bool goalScored = false;
	std::set<Ball *> currentBallCollisions;
	int ticksSinceGrounded = 0;
	bool isGrounded = false;
	Player * lastHitBy = nullptr;
};