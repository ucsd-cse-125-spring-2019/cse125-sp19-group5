#pragma once
#include "GameObject.h"
#include "Paddle.h"
#include "Player.h"
#include "Wall.h"

class Ball : public GameObject {
public:
	using GameObject::GameObject;

	Ball(vec3 position, vec3 velocity, int id, float radius);
	GAMEOBJECT_TYPES getGameObjectType() const;
	void updateOnServerTick();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);
};