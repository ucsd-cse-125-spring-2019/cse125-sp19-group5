#pragma once

#include "Common.h"

class PhysicsEngine
{
public:
	static vec3 acceleratePlayer(vec3 accelDir, vec3 prevVelocity, float accelerate, float maxVelocity, float moveSpeed);
	static vec3 movePlayerOnGround(vec3 accelDir, vec3 prevVelocity, float moveSpeed);
	static vec3 movePlayerInAir(vec3 accelDir, vec3 prevVelocity, float moveSpeed);
	static vec3 jumpPlayer(vec3 prevVelocity);
	static vec3 applyGravity(vec3 prevVelocity, float gravity);
	static vec3 applyFriction(vec3 prevVelocity, float friction);

	static float getDeltaTime()					{ return PhysicsEngine::deltaTime; }
	static float getFloorY()					{ return PhysicsEngine::floorY; }
	static float getGravity()					{ return PhysicsEngine::gravity; }
	static float getPlayerDefaultMoveSpeed()	{ return PhysicsEngine::playerDefaultMoveSpeed; }
	static float getPlayerAccelerationGround()	{ return PhysicsEngine::playerAccelerationGround; }
	static float getPlayerMaxVelocityGround()	{ return PhysicsEngine::playerMaxVelocityGround; }
	static float getPlayerAccelerationAir()		{ return PhysicsEngine::playerAccelerationAir; }
	static float getPlayerMaxVelocityAir()		{ return PhysicsEngine::playerMaxVelocityAir; }
	static float getPlayerMoveFriction()		{ return PhysicsEngine::playerMoveFriction; }
	static float getPlayerJumpSpeed()			{ return PhysicsEngine::playerJumpSpeed; }
	static float getAutoBhopEnabled()			{ return PhysicsEngine::autoBhopEnabled; }

	static void setDeltaTime(float dt)									{ PhysicsEngine::deltaTime = dt; }
	static void setFloorY(float floorY)									{ PhysicsEngine::floorY = floorY; }
	static void setGravity(const float gravity)							{ PhysicsEngine::gravity = gravity; }
	static void setPlayerAccelerationGround(const float acceleration)	{ PhysicsEngine::playerAccelerationGround = acceleration; }
	static void setPlayerMaxVelocityGround(const float maxVelocity)		{ PhysicsEngine::playerMaxVelocityGround = maxVelocity; }
	static void setPlayerAccelerationAir(const float acceleration)		{ PhysicsEngine::playerAccelerationAir = acceleration; }
	static void setPlayerMaxVelocityAir(const float maxVelocity)		{ PhysicsEngine::playerMaxVelocityAir = maxVelocity; }
	static void setPlayerMoveFriction(const float friction)				{ PhysicsEngine::playerMoveFriction = friction; }
	static void setPlayerJumpSpeed(const float jumpSpeed)				{ PhysicsEngine::playerJumpSpeed = jumpSpeed; }
	static void setAutoBhopEnabled(const bool enabled)					{ PhysicsEngine::autoBhopEnabled = enabled; }

private:
	static float deltaTime; // ms
	static float floorY;
	static float gravity;
	static float playerDefaultMoveSpeed;
	static float playerAccelerationGround;
	static float playerMaxVelocityGround;
	static float playerAccelerationAir;
	static float playerMaxVelocityAir;
	static float playerMoveFriction;
	static float playerJumpSpeed;
	static bool autoBhopEnabled;
};
