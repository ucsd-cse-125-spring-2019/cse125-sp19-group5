#include "Sound.h"

Sound::Sound(): sound(nullptr) {
}

Sound::Sound(irrklang::ISound &sound, float volume, const bool isSpatial)
	: sound(&sound)
	, isSpatial(isSpatial) {
	setVolume(volume);
}

Sound::~Sound() {
	if (sound) {
		sound->drop();
		sound = nullptr;
	}
}

void Sound::play(const bool isLooping) {
	if (!sound) { return; }
	sound->setIsPaused(false);
	sound->setIsLooped(isLooping);
}

void Sound::pause() {
	if (!sound) { return; }
	sound->setIsPaused(true);
}

void Sound::setVolume(float volume) {
	if (!sound) { return; }
	sound->setVolume(volume);
}

void Sound::setPosition(vec3 pos) {
	if (!sound || !isSpatial) { return; }
	irrklang::vec3df soundPos = irrklang::vec3df(pos.x, pos.y, pos.z);
	sound->setPosition(soundPos);
}

void Sound::setVelocity(vec3 vel) {
	if (!sound || !isSpatial) { return; }
	irrklang::vec3df soundVel = irrklang::vec3df(vel.x, vel.y, vel.z);
	sound->setVelocity(soundVel);
}

void Sound::setMinDist(float dist) {
	if (!sound || !isSpatial) { return; }
	sound->setMinDistance(dist);
}

void Sound::setMaxDist(float dist) {
	if (!sound || !isSpatial) { return; }
	sound->setMaxDistance(dist);
}