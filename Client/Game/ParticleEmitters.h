#pragma once

#include <unordered_map>
#include <Shared/Game/ParticleEmitter.h>
#include "../Networking/Client.h"
#include "../Renderer/ParticleSystem.h"
#include "../Renderer/Camera.h"

using std::unordered_map;

namespace ParticleEmitters {
	extern unordered_map<size_t, ParticleSystem*> systems;

	void onUpdate(Connection *c, NetBuffer &buffer);
	void onDelete(Connection *c, NetBuffer &buffer);
	void update(float dt, const Camera *camera);
	void draw(const Camera *camera);
	void cleanUp();
}