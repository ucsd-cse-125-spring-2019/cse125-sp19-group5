#pragma once

#include "../Common.h"
#include "../Networking/Serializable.h"

#define PARTICLE_PARAM(type, var) \
		type _##var; \
		void set##var(type v) { \
			_##var = v; \
			update(); \
		}

class ParticleEmitter : public Serializable {
	private:
	static size_t nextId; // Unique number for the next emitter.

	public:
	PARTICLE_PARAM(unsigned int, CreationSpeed)
	unsigned int creationSpeed;   // Particles per second
	vec3 initialPos;              // Initial position
	vec3 initialPosVariance;      // +/- from initial position
	vec3 initialVel;              // Initial velocity
	vec3 initialVelVariance;      // +/- from initial velocity
	float initialLife;            // Initial life time
	float initialLifeVariance;    // +/- from initial life time
	float gravity;                // Constant -y force
	float airDensity;             // For aerodynamic drag
	float dragCoeff;              // For aerodynamic drag
	float particleRadius;         // For aerodynamic drag and rendering
	float collElasticity;         // For ground plane collision
	float collFriction;           // For ground plane collision
	vector<vec4> particleColor;   // Color of each particle during its lifetime

	ParticleEmitter();
	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;
	~ParticleEmitter();
};
