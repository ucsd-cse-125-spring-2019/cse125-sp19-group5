#include "PhysicsEngine.h"

// Default values
float PhysicsEngine::deltaTime = 1.0f / 60.0f;
float PhysicsEngine::floorY = 0.0f;
float PhysicsEngine::gravity = 0.005f;
float PhysicsEngine::playerAccelerationGround = 0.005f;
float PhysicsEngine::playerMaxVelocityGround = 0.05f;
float PhysicsEngine::playerAccelerationAir = 0.005f;
float PhysicsEngine::playerMaxVelocityAir = 0.05f;
float PhysicsEngine::playerMoveFriction = 1.0f;
float PhysicsEngine::playerJumpSpeed = 0.02f;
bool PhysicsEngine::autoBhopEnabled = true;

/* accelDir: The normalized direction the player wants to accelerate in,
 *			 accounting for movement keys and lookDirection
 * prevVelocity: Current velocity of player
 * return: New velocity of player
 */
vec3 PhysicsEngine::acceleratePlayer(vec3 accelDir, vec3 prevVelocity, float accelerate, float maxVelocity) {
	float velocityProjection = glm::dot(prevVelocity, accelDir);
	float addSpeed = accelerate * PhysicsEngine::deltaTime;

	if (velocityProjection + addSpeed > maxVelocity)
		addSpeed = maxVelocity - velocityProjection;

	return prevVelocity + accelDir * addSpeed;
}

vec3 PhysicsEngine::movePlayerOnGround(vec3 accelDir, vec3 prevVelocity) {
	return PhysicsEngine::acceleratePlayer(accelDir, prevVelocity, PhysicsEngine::playerAccelerationGround, PhysicsEngine::playerMaxVelocityGround);
}

vec3 PhysicsEngine::movePlayerInAir(vec3 accelDir, vec3 prevVelocity) {
	return PhysicsEngine::deltaTime * PhysicsEngine::acceleratePlayer(accelDir, prevVelocity, PhysicsEngine::playerAccelerationAir, PhysicsEngine::playerMaxVelocityAir);
}

vec3 PhysicsEngine::jumpPlayer(vec3 prevVelocity) {
	prevVelocity.y = 0.0f;
	return prevVelocity + vec3(0.0f, PhysicsEngine::playerJumpSpeed, 0.0f);
}

vec3 PhysicsEngine::applyGravity(vec3 velocity) {
	return velocity - vec3(0.0f, PhysicsEngine::gravity, 0.0f) * PhysicsEngine::deltaTime;
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


