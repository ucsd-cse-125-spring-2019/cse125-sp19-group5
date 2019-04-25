#pragma once
#include "Common.h"

class GameObject {
public:
	GameObject();
	GameObject(vec3 position, vec3 velocity, string id, int radius);
	bool collidesWith(GameObject * gameObject);
	virtual void onCollision(GameObject * gameObject);
	double distanceFrom(GameObject * gameObject);
	vec3 setPosition(vec3 position);
	vec3 getPosition();
	vec3 setVelocity(vec3 velocity);
	vec3 getVelocity();
	string getId();
	string getGameObjectType();
	int getIntId();
	void move(vec3 movement);
	virtual bool deleteOnServerTick();
	virtual void updateOnServerTick();

protected:
	vec3 position;
	vec3 velocity;
	string id;
	int radius;
};