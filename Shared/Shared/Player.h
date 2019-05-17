#pragma once
#include "GameObject.h"
#include "CommonStructs.h"
#include "Ball.h"
#include "Paddle.h"
#include "Wall.h"

class Player : public GameObject {
public:
	using GameObject::GameObject;
  
  Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team);

	void setDirection(const vec3 &newDirection);
	GAMEOBJECT_TYPES getGameObjectType() const;
	vec3 getMoveDestination(vec3 movement);
	vec3 getDirection();
	GameObject * doAction(PlayerCommands action);
	GameObject * processCommand(int inputs);

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);
private:
	vec3 direction;
	float radius;
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;
};