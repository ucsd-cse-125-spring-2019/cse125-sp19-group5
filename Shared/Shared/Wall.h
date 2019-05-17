#pragma once
#include "GameObject.h"
#include "Ball.h"
#include "Paddle.h"
#include "Player.h"

struct Plane;

class Wall : public GameObject {
public:
	using GameObject::GameObject;

	Wall(vec3 position, vec3 direction, int id, float length, float width, float height);
	GAMEOBJECT_TYPES getGameObjectType() const;

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);

private:
	vec3 direction;
	float length;
	float width;
	float height;
};