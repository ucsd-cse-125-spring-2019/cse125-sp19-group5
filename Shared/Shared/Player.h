#pragma once
#include <map>
#include "SphereGameObject.h"
#include "CommonStructs.h"
#include "Ball.h"
#include "Bullet.h"
#include "Paddle.h"
#include "Wall.h"
#include "PhysicsEngine.h"

class Player : public SphereGameObject {
public:
	using SphereGameObject::SphereGameObject;
  
	Player(vec3 position, vec3 velocity, vec3 direction, int id, float radius, int team);

	GAMEOBJECT_TYPES getGameObjectType() const;
	void setDirection(const vec3 &newDirection);
	void updateOnServerTick();
	vec3 getMoveDestination(vec3 movement);
	vec3 getDirection();

	tuple<int, int> & getCooldown(PlayerCommands command);
	void setCooldown(PlayerCommands command, tuple<int, int> cd);
	void useCooldown(PlayerCommands command);
	GameObject * doAction(PlayerCommands action);
	GameObject * processCommand(int inputs);

	bool getIsGrounded() const { return isGrounded; }
	void setIsGrounded(const bool grounded) { isGrounded = grounded; }

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
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;

	// get<0> = current cooldown, get<1> = total cooldown
	std::map<PlayerCommands, tuple<int, int>> cooldowns;
	float moveSpeed = PhysicsEngine::getPlayerDefaultMoveSpeed();
	bool isGrounded = false;
	bool isJumpKeyDown = false; // For bhop
	bool isJumpKey = false;
	bool isJumpKeyUp = false;
};