#include "ParticleEmitters.h"
#include "../Assets.h"

unordered_map<size_t, ParticleSystem*> ParticleEmitters::systems;
GameState *ParticleEmitters::gameState = nullptr;
static Shader *psShader = nullptr;

void ParticleEmitters::init(GameState *newGameState) {
	gameState = newGameState;
}

void ParticleEmitters::onUpdate(Connection *c, NetBuffer &buffer) {
	const auto id = buffer.read<size_t>();
	ParticleEmitter emitter;
	emitter.deserialize(buffer);

	if (systems.find(id) == systems.end()) {
		systems[id] = new ParticleSystem();
	}
	auto system = systems[id];

	system->creationSpeed = emitter._CreationSpeed;
	system->initialPos = emitter._InitialPos;
	system->initialPosVariance = emitter._InitialPosVariance;
	system->initialVel = emitter._InitialVel;
	system->initialVelVariance = emitter._InitialVelVariance;
	system->initialLife = emitter._InitialLife;
	system->initialLifeVariance = emitter._InitialLifeVariance;
	system->gravity = emitter._Gravity;
	system->airDensity = emitter._AirDensity;
	system->dragCoeff = emitter._DragCoeff;
	system->particleRadius = emitter._ParticleRadius;
	system->collElasticity = emitter._CollElasticity;
	system->collFriction = emitter._CollFriction;
	system->particleColor = emitter._ParticleColor;
	system->texture = Assets::getTexture2d(emitter._Texture);
	system->parentId = emitter._ParentId;
	system->creationTime = emitter._CreationTime;
}

void ParticleEmitters::onDelete(Connection *c, NetBuffer &buffer) {
	const auto id = buffer.read<size_t>();
	if (systems.find(id) != systems.end()) {
		delete systems[id];
		systems.erase(id);
	}
}

void ParticleEmitters::update(float dt, const Camera *camera) {
	for (auto &idAndSystem : systems) {
		auto system = idAndSystem.second;

		if (gameState && system->parentId >= 0) {
			auto parent = gameState->gameObjects[system->parentId];
			system->initialPos = parent->getPosition();
		}
		system->update(dt, camera);
	}
}

void ParticleEmitters::draw(const Camera *camera) {
	static auto psShader = new Shader("Shaders/particle");
	for (auto &system : systems) {
		system.second->draw(*psShader, camera);
	}
}

void ParticleEmitters::cleanUp() {
	for (auto system : systems) {
		delete system.second;
	}
}