#include "PhysicsEngine.h"

// Default values
float PhysicsEngine::deltaTime = 1.0f / 60.0f;
float PhysicsEngine::floorY = 0.0f;
float PhysicsEngine::gravity = 0.0005f;
float PhysicsEngine::playerDefaultMoveSpeed = 0.08;
float PhysicsEngine::playerAccelerationGround = 0.02f;
float PhysicsEngine::playerMaxVelocityGround = 0.06f;
float PhysicsEngine::playerAccelerationAir = 0.005f;
float PhysicsEngine::playerMaxVelocityAir = 0.02f;
float PhysicsEngine::playerMoveFriction = 0.8f;
float PhysicsEngine::playerJumpSpeed = 0.09f;
bool PhysicsEngine::autoBhopEnabled = true;

/* accelDir: The normalized direction the player wants to accelerate in,
 *			 accounting for movement keys and lookDirection
 * prevVelocity: Current velocity of player
 * accelerate: How much to accelerate by
 * maxVelocity: Maximum velocity in certain situations (not bhopping)
 * moveSpeed: How much to move by
 * return: New velocity of player
 */
vec3 PhysicsEngine::acceleratePlayer(vec3 accelDir, vec3 prevVelocity, float accelerate, float maxVelocity, float moveSpeed) {
	float addSpeed;
	float accelSpeed;
	float currentSpeed;

	currentSpeed = glm::dot(prevVelocity, accelDir);
	addSpeed = moveSpeed - currentSpeed;
	if (addSpeed <= 0)
		return prevVelocity;

	accelSpeed = accelerate * PhysicsEngine::deltaTime * moveSpeed;
	if (accelSpeed > addSpeed)
		accelSpeed = addSpeed;

	vec3 newSpeed = prevVelocity + (accelDir * accelSpeed);
	return newSpeed;
}

vec3 PhysicsEngine::movePlayerOnGround(vec3 accelDir, vec3 prevVelocity, float moveSpeed) {
	return PhysicsEngine::acceleratePlayer(accelDir, prevVelocity, PhysicsEngine::playerAccelerationGround, PhysicsEngine::playerMaxVelocityGround, moveSpeed);
}

vec3 PhysicsEngine::movePlayerInAir(vec3 accelDir, vec3 prevVelocity, float moveSpeed) {
	return PhysicsEngine::acceleratePlayer(accelDir, prevVelocity, PhysicsEngine::playerAccelerationAir, PhysicsEngine::playerMaxVelocityAir, moveSpeed);
}

vec3 PhysicsEngine::jumpPlayer(vec3 prevVelocity) {
	return vec3(prevVelocity.x, PhysicsEngine::playerJumpSpeed, prevVelocity.z);
}

vec3 PhysicsEngine::applyGravity(vec3 prevVelocity, float gravity) {
	return vec3(prevVelocity.x, prevVelocity.y - gravity * PhysicsEngine::deltaTime, prevVelocity.z);
}

vec3 PhysicsEngine::applyFriction(vec3 prevVelocity, float friction) {
	float speed = glm::length(prevVelocity);
	vec3 newVelocity = prevVelocity;
	if (speed != 0.0f) {
		float drop = speed * friction * PhysicsEngine::deltaTime;
		newVelocity *= glm::max(speed - drop, 0.0f) / speed;
	}
	return newVelocity;
}


