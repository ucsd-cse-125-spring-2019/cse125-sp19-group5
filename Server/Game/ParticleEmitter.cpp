#include "..\..\Shared\Shared\Game\ParticleEmitter.h"
#include <Shared/Game/ParticleEmitter.h>
#include "../Networking/Server.h"
#include <Shared/Util/CurTime.h>

ParticleEmitter::ParticleEmitter()
	: updateNextTick(true)
	, dieTime(-1.0f)
	, _CreationSpeed(50)
	, _InitialPosVariance(vec3(1.0f))
	, _InitialVel(vec3(0.0f, 0.0f, 0.0f))
	, _InitialVelVariance(vec3(5.0f))
	, _InitialLife(1.0f)
	, _InitialLifeVariance(0.8f)
	, _Gravity(0.0f)
	, _AirDensity(0.2f)
	, _DragCoeff(2.0f)
	, _ParticleRadius(0.8f)
	, _CollElasticity(0.6f)
	, _CollFriction(0.2f)
	, _ParticleColor({ vec4(1.0f), vec4(1.0f, 1.0f, 1.0f, 0.0f) })
	, _Texture("Textures/white.png")
	, _ParentId(-1)
	, parent(nullptr)
	, _CreationTime(1.0f)
{
	id = nextId;
	nextId++;
	emitters.push_back(this);
}

void ParticleEmitter::update() {
	if (dieTime > 0 && dieTime <= curTime()) {
		NetBuffer buffer(NetMessage::PARTICLES_DELETE);
		buffer.write(id);
		Network::broadcast(buffer);

		delete this;
		return;
	}
	if (updateNextTick) {
		Network::broadcast(NetMessage::PARTICLES, *this);
		updateNextTick = false;
	}
}

void ParticleEmitter::updateAll() {
	auto oldEmitters = emitters;
	for (auto emitter : oldEmitters) {
		emitter->update();
	}
}

void ParticleEmitter::setLifeTime(float seconds) {
	dieTime = curTime() + seconds;
}

void ParticleEmitter::deleteAll() {
	auto toDelete = emitters;
	emitters.clear();
	for (auto emitter : toDelete) {
		delete emitter;
	}
}

void ParticleEmitter::setParent(GameObject *newParent) {
	parent = newParent;
	setParentId(parent ? parent->getId() : -1);
}

GameObject *ParticleEmitter::getParent() const {
	return parent;
}

ParticleEmitter::~ParticleEmitter() {
	auto it = std::find(emitters.begin(), emitters.end(), this);
	if (it != emitters.end()) {
		emitters.erase(it);
	}
}