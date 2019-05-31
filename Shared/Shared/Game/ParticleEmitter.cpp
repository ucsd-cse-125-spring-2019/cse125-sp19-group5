#include "ParticleEmitter.h"

size_t ParticleEmitter::nextId = 0;
vector<ParticleEmitter*> ParticleEmitter::emitters;

void ParticleEmitter::serialize(NetBuffer &buffer) const {
	buffer.write(id); // Note: this is assumed to be read indep.
	buffer.write(_CreationSpeed);
	buffer.write(_InitialPos);
	buffer.write(_InitialPosVariance);
	buffer.write(_InitialVel);
	buffer.write(_InitialVelVariance);
	buffer.write(_InitialLife);
	buffer.write(_InitialLifeVariance);
	buffer.write(_Gravity);
	buffer.write(_AirDensity);
	buffer.write(_DragCoeff);
	buffer.write(_ParticleRadius);
	buffer.write(_CollElasticity);
	buffer.write(_CollFriction);
	buffer.write(_Texture);
	buffer.write(_CreationTime);
	buffer.write(_ParticleColor.size());
	for (auto &color : _ParticleColor) {
		buffer.write(color);
	}
}

void ParticleEmitter::deserialize(NetBuffer &buffer) {
	_CreationSpeed = buffer.read<unsigned int>();
	_InitialPos = buffer.read<vec3>();
	_InitialPosVariance = buffer.read<vec3>();
	_InitialVel = buffer.read<vec3>();
	_InitialVelVariance = buffer.read<vec3>();
	_InitialLife = buffer.read<float>();
	_InitialLifeVariance = buffer.read<float>();
	_Gravity = buffer.read<float>();
	_AirDensity = buffer.read<float>();
	_DragCoeff = buffer.read<float>();
	_ParticleRadius = buffer.read<float>();
	_CollElasticity = buffer.read<float>();
	_CollFriction = buffer.read<float>();
	_Texture = buffer.read<string>();
	_CreationTime = buffer.read<float>();

	const auto numColors = buffer.read<size_t>();
	_ParticleColor.resize(numColors);
	for (size_t i = 0; i < numColors; i++) {
		_ParticleColor[i] = buffer.read<vec4>();
	}
}
