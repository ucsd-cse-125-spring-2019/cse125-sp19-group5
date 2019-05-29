#include "SoundEngine.h"

const std::string ERRKLANG_ERROR_MSG = "Error: irrKlang audio failed to initialize.";

SoundEngine::SoundEngine() {
	engine = irrklang::createIrrKlangDevice();
	engine->setDefault3DSoundMinDistance(3.0f);
	if (!engine) {
		std::cout << ERRKLANG_ERROR_MSG << std::endl;
		didInitialize = false;
	}
	else {
		didInitialize = true;
	}
}

SoundEngine::~SoundEngine() {
	if (engine) engine->drop();
}

bool SoundEngine::isInitialized() {
	return didInitialize;
}

Sound *SoundEngine::loadFlatSound(const std::string &filepath, float volume) {
	if (!isInitialized()) {
		return new Sound();
	}
	irrklang::ISound *sound = engine->play2D(filepath.c_str(), false, true, true);
	return new Sound(*sound, volume, false);
}

Sound *SoundEngine::loadSpatialSound(const std::string &filepath, float volume) {
	if (!isInitialized()) {
		return new Sound();
	}
	irrklang::vec3df origin = irrklang::vec3df(0.0f, 0.0f, 0.0f);
	irrklang::ISound * sound = engine->play3D(filepath.c_str(), origin, false, true, true);
	return new Sound(*sound, volume, true);
}

void SoundEngine::setMasterVolume(float volume) {
	if (!isInitialized()) return;

	engine->setSoundVolume(volume);
}

void SoundEngine::update(vec3 pos, vec3 vel, vec3 lookDir) {
	if (!isInitialized()) return;

	irrklang::vec3df position = irrklang::vec3df(pos.x, pos.y, pos.z);
	irrklang::vec3df velocity = irrklang::vec3df(vel.x, vel.y, vel.z);
	irrklang::vec3df lookAt = irrklang::vec3df(lookDir.x, lookDir.y, lookDir.z);

	engine->setListenerPosition(position, lookAt, velocity);
	engine->update();
}

SoundEngine *gSound = new SoundEngine();