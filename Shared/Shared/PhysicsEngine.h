#pragma once

#include "Common.h"

class PhysicsEngine
{
public:
	static vec3 acceleratePlayer(vec3 accelDir, vec3 prevVelocity, float accelerate, float maxVelocity);
	static vec3 movePlayerOnGround(vec3 accelDir, vec3 prevVelocity);
	static vec3 movePlayerInAir(vec3 accelDir, vec3 prevVelocity);
	static vec3 applyGravity(vec3 velocity);

	static float getDeltaTime()					{ return PhysicsEngine::deltaTime; }
	static float getGravity()					{ return PhysicsEngine::gravity; }
	static float getPlayerAccelerationGround()	{ return PhysicsEngine::playerAccelerationGround; }
	static float getPlayerMaxVelocityGround()	{ return PhysicsEngine::playerMaxVelocityGround; }
	static float getPlayerAccelerationAir()		{ return PhysicsEngine::playerAccelerationAir; }
	static float getPlayerMaxVelocityAir()		{ return PhysicsEngine::playerMaxVelocityAir; }
	static float getPlayerMoveFriction()		{ return PhysicsEngine::playerMoveFriction; }

	static void setDeltaTime(float dt)									{ PhysicsEngine::deltaTime = dt; }
	static void setGravity(const float gravity)							{ PhysicsEngine::gravity = gravity; }
	static void setPlayerAccelerationGround(const float acceleration)	{ PhysicsEngine::playerAccelerationGround = acceleration; }
	static void setPlayerMaxVelocityGround(const float maxVelocity)		{ PhysicsEngine::playerMaxVelocityGround = maxVelocity; }
	static void setPlayerAccelerationAir(const float acceleration)		{ PhysicsEngine::playerAccelerationAir = acceleration; }
	static void setPlayerMaxVelocityAir(const float maxVelocity)		{ PhysicsEngine::playerMaxVelocityAir = maxVelocity; }
	static void setPlayerMoveFriction(const float friction)				{ PhysicsEngine::playerMoveFriction = friction; }

private:
	static float deltaTime; // ms
	static float gravity;
	static float playerAccelerationGround;
	static float playerMaxVelocityGround;
	static float playerAccelerationAir;
	static float playerMaxVelocityAir;
	static float playerMoveFriction;
};
