#pragma once
#include "BoxGameObject.h"
#include "Ball.h"
#include "Paddle.h"
#include "Player.h"


class Wall : public BoxGameObject {
public:
	using BoxGameObject::BoxGameObject;
	GAMEOBJECT_TYPES getGameObjectType() const;

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);
};