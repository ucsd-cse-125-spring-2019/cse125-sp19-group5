#pragma once
#include "Common.h"
#include "Serializable.h"
class GameObject : public Serializable {
public:
	GameObject(vec3 position, vec3 velocity, string id, int radius);
	bool collidesWith(GameObject &gameObject);
	virtual void onCollision(GameObject &gameObject);
	double distanceFrom(GameObject &gameObject);
	vec3 setPosition(vec3 position);
	vec3 getPosition();
	vec3 setVelocity(vec3 velocity);
	vec3 getVelocity();
	string getId();
	string getGameObjectType() const;
	void move(vec3 movement);
	void serialize(NetBuffer &buffer) const;
	static GameObject deserialize(NetBuffer &buffer);

protected:
	vec3 position;
	vec3 velocity;
	string id;
	int radius;
};