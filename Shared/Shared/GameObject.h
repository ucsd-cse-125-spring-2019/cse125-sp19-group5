#pragma once
#include "Common.h"
#include "Networking/Serializable.h"

class GameObject : public Serializable {
public:
	GameObject();
	GameObject(const string &id);
	GameObject(vec3 position, vec3 velocity, string id, int radius);
	bool collidesWith(GameObject * gameObject);
	virtual void onCollision(GameObject * gameObject);
	double distanceFrom(GameObject * gameObject);
	vec3 setPosition(vec3 position);
	vec3 getPosition();
	vec3 setVelocity(vec3 velocity);
	vec3 getVelocity();
	void setScale(const vec3 &newScale);
	vec3 getScale() const;
	string getId();
	string getGameObjectType() const;
	int getIntId();
	void move(vec3 movement);
	void updateOnServerTick();
	bool deleteOnServerTick();
	void serialize(NetBuffer &buffer) const;
	void deserialize(NetBuffer &buffer);

protected:
	vec3 position;
	vec3 velocity;
	vec3 scale = vec3(1.0f);
	string id;
	int radius;
};