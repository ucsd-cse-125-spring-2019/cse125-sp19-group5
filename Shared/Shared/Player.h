#pragma once
#include <map>
#include "SphereGameObject.h"
#include "BoundingSphere.h"
#include "Ball.h"
#include "Bullet.h"
#include "Paddle.h"
#include "PowerUpItem.h"
#include "Wall.h"
#include "CommonStructs.h"
#include "PhysicsEngine.h"

class Powerup;
#include "Game/Powerup.h"
#include <unordered_map>

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

	void doAction(PlayerCommands action);
	void processCommand(int inputs);

	bool getIsGrounded() const { return isGrounded; }
	void setIsGrounded(const bool grounded) { isGrounded = grounded; }

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Bullet * bullet);
	void onCollision(Goal * goal);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(PowerUpItem * item);
	void onCollision(Wall * wall);

	// Powerups
	template<class T>
	void addPowerup() {
		const string &type = T::TYPE;
		T *powerup = new T(*this);
		auto it = powerups.find(type);
		if (it != powerups.end()) {
			removePowerup(type);
		}

		powerups[type] = powerup;
		powerup->activate();
		powerup->onActivate();
	}

	void addPowerup(POWERUP_TYPES powerup);

	void removePowerup(const string &type);

	bool hasPowerup(const string &type) const;

	void setMoveSpeed(float newMoveSpeed);
	float getMoveSpeed() const;

private:
	vec3 direction;
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;
	std::unordered_map<string, Powerup*> powerups;

	// get<0> = current cooldown, get<1> = total cooldown
	std::map<PlayerCommands, tuple<int, int>> cooldowns;

	float moveSpeed = PhysicsEngine::getPlayerDefaultMoveSpeed();
	bool isGrounded = false;
	bool isJumpKeyDown = false; // For bhop
	bool isJumpKey = false;
	bool isJumpKeyUp = false;
	float maxBoxHeight = 0.0f;
	int numLandings = 0;
	vec3 collisionVelocityComponent = vec3(0);
	vec3 prevPosition;
};