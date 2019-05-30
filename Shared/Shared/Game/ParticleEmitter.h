#pragma once

#include "../Common.h"
#include "../Networking/Serializable.h"

// Defines a member variable with a setter that synchronizes the state on client.
#define PARTICLE_PARAM(type, var) \
		type _##var; \
		void set##var(type v) { \
			_##var = v; \
			updateNextTick = true; \
		}

class ParticleEmitter : public Serializable {
	private:
	static vector<ParticleEmitter*> emitters;
	bool updateNextTick;  // Whether or not a state update message is needed.
	size_t id;            // A unique number used to identify this emitter.
	static size_t nextId; // Unique number for the next emitter.
	float dieTime;        // When the emitter should be auto-deleted. -1 = never

	public:
	PARTICLE_PARAM(unsigned int, CreationSpeed);
	PARTICLE_PARAM(vec3, InitialPos);
	PARTICLE_PARAM(vec3, InitialPosVariance);
	PARTICLE_PARAM(vec3, InitialVel);
	PARTICLE_PARAM(vec3, InitialVelVariance);
	PARTICLE_PARAM(float, InitialLife);
	PARTICLE_PARAM(float, InitialLifeVariance);
	PARTICLE_PARAM(float, Gravity);
	PARTICLE_PARAM(float, AirDensity);
	PARTICLE_PARAM(float, DragCoeff);
	PARTICLE_PARAM(float, ParticleRadius);
	PARTICLE_PARAM(float, CollElasticity);
	PARTICLE_PARAM(float, CollFriction);
	PARTICLE_PARAM(vector<vec4>, ParticleColor);
	PARTICLE_PARAM(string, Texture);
	PARTICLE_PARAM(float, CreationTime);

	ParticleEmitter();

	void serialize(NetBuffer &buffer) const override;
	void deserialize(NetBuffer &buffer) override;

	// Sends state to client so it can be synchronized.
	void update() const;  

	// Sets how long this emitter stays active for.
	void setLifeTime(float seconds); 

	~ParticleEmitter();

	// Calls update() on all existing ParticleEmitters.
	static void updateAll();
};
