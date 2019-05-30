#pragma once
#include "GameObject.h"

class Paddle : public GameObject {
public:
	using GameObject::GameObject;

	Paddle(vec3 position, vec3 velocity, int id, float radius, int lifespan);
	GAMEOBJECT_TYPES getGameObjectType() const;
	bool deleteOnServerTick();
	void updateOnServerTick();

	void setLifespan(int newLifespan);
	int getLifespan() const;

	void onCreated() override;

private:
	int lifespan;
};