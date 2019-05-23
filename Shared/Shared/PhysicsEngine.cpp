// https://flafla2.github.io/2015/02/14/bunnyhop.html

#include "PhysicsEngine.h"

// Default values
float PhysicsEngine::deltaTime = 1.0f / 60.0f;
float PhysicsEngine::gravity = 1.0f;
float PhysicsEngine::playerAccelerationGround = 0.005f;
float PhysicsEngine::playerMaxVelocityGround = 0.05f;
float PhysicsEngine::playerAccelerationAir = 0.005f;
float PhysicsEngine::playerMaxVelocityAir = 0.05f;
float PhysicsEngine::playerMoveFriction = 1.0f;

/* accelDir: The normalized direction the player wants to accelerate in,
 *			 accounting for movement keys and lookDirection
 * prevVelocity: Current velocity of player
 * return: New velocity of player
 */
vec3 PhysicsEngine::acceleratePlayer(vec3 accelDir, vec3 prevVelocity, float accelerate, float maxVelocity) {
	float velocityProjection = glm::dot(prevVelocity, accelDir);
	float accelAmt = accelerate * PhysicsEngine::deltaTime;

	if (velocityProjection + accelAmt > maxVelocity)
		accelAmt = maxVelocity - velocityProjection;

	return prevVelocity + accelDir * accelAmt;
}

vec3 PhysicsEngine::movePlayerOnGround(vec3 accelDir, vec3 prevVelocity) {
	float speed = glm::length(prevVelocity);
	if (speed != 0.0f) {
		float drop = speed * PhysicsEngine::playerMoveFriction * PhysicsEngine::deltaTime;
		prevVelocity *= glm::max(speed - drop, 0.0f) / speed;
	}

	return PhysicsEngine::acceleratePlayer(accelDir, prevVelocity, PhysicsEngine::playerAccelerationGround, PhysicsEngine::playerMaxVelocityGround);
}

vec3 PhysicsEngine::movePlayerInAir(vec3 accelDir, vec3 prevVelocity) {
	return PhysicsEngine::deltaTime * PhysicsEngine::acceleratePlayer(accelDir, prevVelocity, PhysicsEngine::playerAccelerationAir, PhysicsEngine::playerMaxVelocityAir);
}

vec3 PhysicsEngine::applyGravity(vec3 velocity) {
	return velocity - vec3(0.0f, PhysicsEngine::gravity, 0.0f) * PhysicsEngine::deltaTime;
}


