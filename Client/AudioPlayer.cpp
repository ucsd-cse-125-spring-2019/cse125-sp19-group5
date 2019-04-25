#include "AudioPlayer.h"

const std::string ERRKLANG_ERROR_MSG = "Error: irrKlang audio failed to initialize.";

AudioPlayer::AudioPlayer() {
	engine = irrklang::createIrrKlangDevice();
	if (!engine) {
		std::cout << ERRKLANG_ERROR_MSG << std::endl;
		didInitialize = false;
	}
	else {
		didInitialize = true;
	}
}

AudioPlayer::~AudioPlayer() {
	if (engine) engine->drop();
}

bool AudioPlayer::isInitialized() {
	if (!didInitialize)
		std::cout << ERRKLANG_ERROR_MSG << std::endl;

	return didInitialize;
}

void AudioPlayer::playLoop(const std::string &filepath) {
	if (!isInitialized()) return;
	engine->play2D(filepath.c_str(), true);
}
