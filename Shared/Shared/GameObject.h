#pragma once
#include "Common.h"
#include "Networking/Serializable.h"
#include "BoundingShape.h"

class GameObject : public Serializable {
public:
	GameObject();
	GameObject(const int &id);
	GameObject(vec3 position, vec3 velocity, int id);

	BoundingShape * getBoundingShape();
	void setBoundingShape(BoundingShape * boundingShape);

	vec3 setPosition(vec3 position);
	vec3 getPosition();

	vec3 setVelocity(vec3 velocity);
	vec3 getVelocity();

	void setScale(const vec3 &newScale); 
	vec3 getScale() const;

	virtual int getGameObjectType() const;
	int getId();

	virtual vec3 getMoveDestination(vec3 movement);
	void move(vec3 movement);

	bool collidesWith(GameObject * gameObject);
	virtual void onCollision(GameObject * gameObject);
	double distanceFrom(GameObject * gameObject);

	virtual void updateOnServerTick();
	virtual bool deleteOnServerTick();

	void serialize(NetBuffer &buffer) const;
	void deserialize(NetBuffer &buffer);

	virtual string to_string();

protected:
	vec3 position;
	vec3 velocity;
	vec3 scale = vec3(1.0f);
	int id;
	BoundingShape * boundingShape;
};

enum GAMEOBJECT_TYPES {
	GAMEOBJECT_TYPE,
	PLAYER_TYPE,
	BALL_TYPE,
	WALL_TYPE,
	PADDLE_TYPE,
};