#pragma once
#include "Common.h"
#include "Networking/Serializable.h"

enum GAMEOBJECT_TYPES {
	GAMEOBJECT_TYPE,
	PLAYER_TYPE,
	BALL_TYPE,
	WALL_TYPE,
	PADDLE_TYPE,
};

class GameObject : public Serializable {
public:
	GameObject();
	GameObject(const int &id);
	GameObject(vec3 position, vec3 velocity, int id, int radius);
	bool collidesWith(GameObject * gameObject);
	virtual void onCollision(GameObject * gameObject);
	double distanceFrom(GameObject * gameObject);
	vec3 setPosition(vec3 position);
	vec3 getPosition();
	vec3 setVelocity(vec3 velocity);
	vec3 getVelocity();
	void setScale(const vec3 &newScale);
	vec3 getScale() const;
	int getRadius();
	virtual GAMEOBJECT_TYPES getGameObjectType() const = 0;
	int getId();
	virtual vec3 getMoveDestination(vec3 movement);
	void move(vec3 movement);
	virtual void updateOnServerTick();
	virtual bool deleteOnServerTick();
	void serialize(NetBuffer &buffer) const;
	void deserialize(NetBuffer &buffer);
	virtual string to_string();

	void setModel(const string &newModel);
	string getModel() const;

#ifdef _SERVER
	public:
	void setAnimation(int id = -1, bool reset = true);
	int getAnimation() const;
	protected:
	int animation = -1;
#endif

protected:
	string model = "";
	vec3 position;
	vec3 velocity;
	vec3 scale = vec3(1.0f);
	int id;
	int radius;
};
