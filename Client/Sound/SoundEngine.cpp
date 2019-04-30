#include "SoundEngine.h"

const std::string ERRKLANG_ERROR_MSG = "Error: irrKlang audio failed to initialize.";

SoundEngine::SoundEngine() {
	engine = irrklang::createIrrKlangDevice();
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
	if (!didInitialize)
		std::cout << ERRKLANG_ERROR_MSG << std::endl;

	return didInitialize;
}

Sound *SoundEngine::loadFlatSound(const std::string &filepath) {
	if (!isInitialized()) {
		return new Sound();
	}
	irrklang::ISound * sound = engine->play2D(filepath.c_str(), false, true, true, irrklang::ESM_AUTO_DETECT, true);
	return new Sound(filepath, *sound);
}

Sound *SoundEngine::loadSpatialSound(const std::string &filepath, const vec3 pos) {
	if (!isInitialized()) {
		return new Sound();
	}
	irrklang::vec3df soundPos = irrklang::vec3df(pos.x, pos.y, pos.z);
	irrklang::ISound * sound = engine->play3D(filepath.c_str(), soundPos, false, true, true, irrklang::ESM_AUTO_DETECT, true);
	return new Sound(filepath, *sound);
}
