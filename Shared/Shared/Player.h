#pragma once

#include "GameObject.h"
#include "Wall.h"
#include "CommonStructs.h"
#include "Paddle.h"
#include "PhysicsEngine.h"

class Player : public GameObject {
public:
	using GameObject::GameObject;
  
	Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team);

	void setDirection(const vec3 &newDirection);
	GAMEOBJECT_TYPES getGameObjectType() const;
	void onCollision(GameObject * gameObject);
	vec3 getMoveDestination(vec3 movement);
	vec3 getDirection();
	void doAction(PlayerCommands action);
	void processCommand(int inputs);

	bool getIsGrounded() const { return isGrounded; }
	void setIsGrounded(const bool grounded) { isGrounded = grounded; }

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;
private:
	vec3 direction;
	float radius;
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;

	float moveSpeed = PhysicsEngine::getPlayerDefaultMoveSpeed();
	bool isGrounded = false;
	bool isJumpKeyDown = false; // For bhop
	bool isJumpKey = false;
	bool isJumpKeyUp = false;
};