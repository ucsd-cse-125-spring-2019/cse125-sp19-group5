#pragma once
#include "BoxGameObject.h"

class Goal : public BoxGameObject {
public:
	using BoxGameObject::BoxGameObject;
	GAMEOBJECT_TYPES getGameObjectType() const;
	void setTeam(int team);
	int & getTeam();
	int & getGoalsScored();
	void setBallSpawnRange(tuple<float, float> xRange, tuple<float, float> yRange, tuple<float, float> zRange);
	vec3 getRandomSpawnPos();

	// collision stuff below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);

private:
	int team;
	int goalsScored = 0;
	tuple<float, float> xRange;
	tuple<float, float> yRange;
	tuple<float, float> zRange;
};