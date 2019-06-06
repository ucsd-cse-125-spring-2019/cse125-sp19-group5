#pragma once
#include "BoxGameObject.h"
#include "Ball.h"
#include "Bullet.h"
#include "Paddle.h"
#include "Player.h"


class Wall : public BoxGameObject {
public:
	using BoxGameObject::BoxGameObject;
	GAMEOBJECT_TYPES getGameObjectType() const;
	bool deleteOnServerTick();
	void setDestroyable(bool destroyable);
	void setHealth(int health);

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Bullet * bullet);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(Wall * wall);
	void onCreated() override;

private:
	vec3 direction;
	float length;
	float width;
	float height;
	bool destroyable = false;
	int health = 1;
};