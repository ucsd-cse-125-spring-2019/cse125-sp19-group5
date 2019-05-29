#pragma once

#include <Shared/Common.h>

struct Particle {
	float mass;
	vec3 position;
	vec3 velocity;
	vec3 force;
	float lifespan;
	float maxLifespan;
	vec4 color;
	float camDist;

	Particle(float mass, vec3 position)
		: mass(mass)
		, position(position)
		, velocity(vec3(1.0f))
		, force(vec3(1.0f))
		, lifespan(1.0f)
		, maxLifespan(1.0f)
		, color(vec4(1.0f))
		, camDist(-1.0f) {}

	void applyForce(const vec3 &newForce) { force += newForce; }
	void resetForce() { force = vec3(0.0f); }

	void update(float dt, float floorY, float collElas, float collFric) {
		lifespan -= dt;

		vec3 accel = (1.0f / mass) * force;
		velocity += accel * dt;
		position += velocity * dt;

		if (position.y < floorY) {
			position.y = 2.0f * floorY - position.y;
			velocity.y = -collElas * velocity.y;
			velocity.x = (1.0f - collFric) * velocity.x;
			velocity.z = (1.0f - collFric) * velocity.z;
		}

		resetForce();
	}
};