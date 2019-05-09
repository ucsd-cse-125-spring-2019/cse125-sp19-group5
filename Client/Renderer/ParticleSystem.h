#pragma once

#include <Shared/Common.h>
#include "Particle.h"
#include "Shader.h"	
#include "Camera.h"

class ParticleSystem {
public:
	ParticleSystem();
	ParticleSystem(const unsigned int maxParticles, const float particleMass, const vec3 position, const float floorY);
	~ParticleSystem();

	void update(float dt);
	void draw(Shader &shader, const Camera *camera);

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

private:
	void setupBuffers();
	vec4 getColorFromLifespan(float lifespan, float maxLifespan);

	unsigned int maxParticles;
	float particleMass;
	float floorY;

	unsigned int numParticles;
	vector<Particle *> particles;
	float creationTimer;          // Used to calculate how many to spawn

	GLuint particleVertexBuffer;
	GLuint particlePositionBuffer;
	GLuint particleColorBuffer;
	vector<GLfloat> particleVertexBufferData;
};