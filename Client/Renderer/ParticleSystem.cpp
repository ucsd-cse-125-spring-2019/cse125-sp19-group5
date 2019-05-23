#include "ParticleSystem.h"
#include <glm/gtx/string_cast.hpp>
#include "../Assets.h"
#include <algorithm>

ParticleSystem::ParticleSystem()
	: ParticleSystem(1000, 1.0f, vec3(0.0f), -2.0f) {
}

ParticleSystem::ParticleSystem(const unsigned int maxParticles, const float particleMass, const vec3 position, const float floorY)
	: maxParticles(maxParticles)
	, particleMass(particleMass)
	, floorY(floorY)
	, creationSpeed(50)
	, initialPos(position)
	, initialPosVariance(vec3(1.0f))
	, initialVel(vec3(0.0f, 0.0f, 0.0f))
	, initialVelVariance(vec3(5.0f))
	, initialLife(1.0f)
	, initialLifeVariance(0.8f)
	, gravity(0.0f)
	, airDensity(0.2f)
	, dragCoeff(2.0f)
	, particleRadius(0.8f)
	, collElasticity(0.6f)
	, collFriction(0.2f)
	, particleColor(vector<vec4>{ vec4(1.0f), vec4(1.0f, 1.0f, 1.0f, 0.0f) })
	, VAO(0)
	, texture(Assets::getTexture2d("Textures/white.png")) {

	for (unsigned int i = 0; i < maxParticles; i++)
	{
		Particle* p = new Particle(particleMass, position);
		particles.push_back(p);
	}

	setupBuffers();
}

ParticleSystem::~ParticleSystem() {
	for (Particle *p : particles) delete p;

	if (VAO) {
		glDeleteVertexArrays(1, &VAO);
	}
}

void ParticleSystem::setupBuffers() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	particleVertexBufferData = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	glGenBuffers(1, &particleVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, particleVertexBufferData.size() * sizeof(GLfloat), particleVertexBufferData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &particlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particleVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad -> 1

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}

void ParticleSystem::update(float dt, const Camera *camera) {
	creationTimer += dt;
	if (creationTimer > 1.0f)
	{
		cout << "Current particle count: " << numParticles << endl;
		for (unsigned int i = 0; i < numParticles && i < 1; i++)
		{
			cout << i << ": " << particles[i]->lifespan << " s" << endl;
			cout << glm::to_string(particles[i]->position) << endl;
			cout << glm::to_string(particles[i]->velocity) << endl;
			cout << glm::to_string(particles[i]->color) << endl;
		}

		creationTimer -= 1.0f;
		for (unsigned int i = 0; i < creationSpeed; i++)
		{
			if (numParticles >= maxParticles) break;

			Particle *newP = particles[numParticles];

			glm::vec3 newPos = initialPos;
			float rx = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			float ry = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			float rz = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			newPos += glm::vec3(rx * initialPosVariance.x,
								ry * initialPosVariance.y,
								rz * initialPosVariance.z);
			newP->position = newPos;
				
			glm::vec3 newVel = initialVel;
			rx = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			ry = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			rz = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			newVel += glm::vec3(rx * initialVelVariance.x,
								ry * initialVelVariance.y,
								rz * initialVelVariance.z);
			newP->velocity = newVel;

			float newLife = initialLife;
			float rl = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2.0f - 1.0f;
			newLife += rl * initialLifeVariance;
			newP->lifespan = newLife;
			newP->maxLifespan = newLife;

			newP->color = particleColor.front();

			numParticles++;
		}
	}

	for (unsigned int i = 0; i < numParticles; i++)
	{
		Particle *currP = particles[i];

		if (currP->lifespan <= 0)
		{
			numParticles--;
			if (numParticles <= 0) break;

			// "Copy" over newest particle
			Particle *newP = particles[numParticles];
			currP->position = newP->position;
			currP->velocity = newP->velocity;
			currP->force = newP->force;
			currP->lifespan = newP->lifespan;
			currP->camDist = newP->camDist;

			newP->camDist = -1.0f;
		}
	}

	// Compute forces
	for (unsigned int i = 0; i < numParticles; i++)
	{
		Particle *p = particles[i];

		// Gravity
		p->applyForce(glm::vec3(0, gravity, 0) * p->mass);

		// Aerodynamic drag = 1/2 * density * |v|^2 * drag * area * e
		float v2 = std::powf(glm::length(p->velocity), 2);
		float area = std::pow(particleRadius * 2, 2);
		glm::vec3 e = -glm::normalize(p->velocity);
		glm::vec3 f = airDensity * v2 * dragCoeff * area * e / 2.0f;
		p->applyForce(f);
	}

	// Integrate
	const auto camPos = camera->getPosition();
	for (unsigned int i = 0; i < numParticles; i++)
	{
		Particle *p = particles[i];
		p->color = getColorFromLifespan(p->lifespan, p->maxLifespan);
		p->update(dt, floorY, collElasticity, collFriction);

		if (p->lifespan > 0) {
			p->camDist = glm::length(p->position - camPos);
		} else {
			p->camDist = -1.0f;
		}
	}

	// Populate buffer data
	std::vector<glm::vec4> particlePositionBufferData(numParticles); // w contains size
	std::vector<glm::vec4> particleColorBufferData(numParticles); // w contains alpha

	std::sort(particles.begin(), particles.end(), [](Particle *a, Particle *b) {
		return a->camDist > b->camDist;
	});

	for (unsigned int i = 0; i < numParticles; i++)
	{
		Particle *p = particles[i];
		glm::vec3 pos = p->position;
		particlePositionBufferData[i] = glm::vec4(pos.x, pos.y, pos.z, particleRadius);
		particleColorBufferData[i] = p->color;
	}

	// Bind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particlePositionBufferData.size() * sizeof(glm::vec4), particlePositionBufferData.data());

	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleColorBufferData.size() * sizeof(glm::vec4), particleColorBufferData.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::draw(Shader &shader, const Camera *camera) {
	glEnable(GL_BLEND);  

	// Set GL matrix state to identity (world)
	glm::mat4x4 modelMtx = glm::mat4(1.0f);

	auto &view = camera->getViewMatrix();
	auto right = vec3(view[0][0], view[1][0], view[2][0]);
	auto up = vec3(view[0][1], view[1][1], view[2][1]);

	// Set up shader
	shader.use();
	shader.setUniform("modelMtx", modelMtx);
	shader.setUniform("camUp", up);
	shader.setUniform("camRight", right);

	// Set up particle texture.
	if (texture) {
		texture->bind(0);
	}
	shader.setUniform("texSampler", 0);

	glm::mat4x4 mvpMtx = camera->getMatrix() * modelMtx;
	shader.setUniform("modelViewProjMtx", mvpMtx);

	// Set up state
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles);
	glBindVertexArray(0);

	glUseProgram(0);
}

vec4 ParticleSystem::getColorFromLifespan(float lifespan, float maxLifespan) {
	float colorFraction = (1.0f - lifespan / maxLifespan) * (particleColor.size() - 1);
	colorFraction = glm::clamp(colorFraction, 0.0f, 1.0f);
	unsigned int firstIndex, secondIndex;
	if (colorFraction == 0.0f) {
		firstIndex = 0;
		secondIndex = 1;
	}
	else if (colorFraction == particleColor.size() - 1) {
		firstIndex = (unsigned int) particleColor.size() - 2;
		secondIndex = (unsigned int) particleColor.size() - 1;
	}
	else {
		firstIndex = (unsigned int) std::floor(colorFraction);
		secondIndex = (unsigned int) std::ceil(colorFraction);
	}

	vec4 firstColor = particleColor[firstIndex];
	vec4 secondColor = particleColor[secondIndex];

	return firstColor + (secondColor - firstColor) * (colorFraction - firstIndex);
}