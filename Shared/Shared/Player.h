#pragma once
#include <map>
#include <set>
#include "SphereGameObject.h"
#include "CommonStructs.h"
#include "PhysicsEngine.h"

enum BULLET_TYPES {
	BULLET_DEFAULT,
	BULLET_STUN
};

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
	void setPositionNoUpdate(vec3 pos);

	tuple<int, int> & getCooldown(PlayerCommands command);
	void setCooldown(PlayerCommands command, tuple<int, int> cd);
	void useCooldown(PlayerCommands command);

	void doAction(PlayerCommands action);
	void processCommand(int inputs);

	bool getIsGrounded() const { return isGrounded; }
	void setIsGrounded(const bool grounded) { isGrounded = grounded; }

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;

	void setGoalsScored(int goals);
	int getGoalsScored();

	// collision interactions below
	void onCollision(GameObject * gameObject);
	void onCollision(Ball * ball);
	void onCollision(Bullet * bullet);
	void onCollision(Goal * goal);
	void onCollision(Paddle * paddle);
	void onCollision(Player * player);
	void onCollision(PowerUpItem * item);
	void onCollision(StunBullet * stunBullet);
	void onCollision(Wall * wall);

	// Powerups
	template<class T>
	void addPowerup() {
		const POWERUP_TYPES &type = T::TYPE;
		T *powerup = new T(*this);
		auto it = powerups.find(type);
		if (it != powerups.end()) {
			removePowerup(type);
		}

		powerups[type] = powerup;
		powerup->activate();
		powerup->onActivate();
	}

	void removePowerup(const POWERUP_TYPES &type);

	bool hasPowerup(const POWERUP_TYPES &type) const;

	void setMoveSpeed(float newMoveSpeed);
	float getMoveSpeed() const;

	void setStrength(float newStrength);
	float getStrength() const;

	void setNumBullets(int numBullets);
	int getNumBullets() const;

	void setBulletType(BULLET_TYPES bulletType);
	BULLET_TYPES getBulletType();

	int getTeam() const;

private:
	vec3 direction;
	int actionCharge;
	int team;
	PlayerCommands currentAction;
	vector<Wall *> walls;
	std::unordered_map<POWERUP_TYPES, Powerup*> powerups;

	// ticks until ball can apply velocity to player again
	std::unordered_map<Ball *, int> currentBallCollisions;

	// get<0> = current cooldown, get<1> = total cooldown
	std::map<PlayerCommands, tuple<int, int>> cooldowns;

	bool isGrounded = false;
	bool isJumpKeyDown = false; // For bhop
	bool isJumpKey = false;
	bool isJumpKeyUp = false;
	float maxBoxHeight = 0.0f;
	int numLandings = 0;
	int goalsScored = 0;
	vec3 collisionVelocityComponent = vec3(0);

	float moveSpeed = PhysicsEngine::getPlayerDefaultMoveSpeed();
	float strength = 1.0f;
	int numBullets = 1;
	BULLET_TYPES bulletType = BULLET_DEFAULT;
};