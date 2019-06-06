#include "RandomSound.h"

RandomSound::RandomSound() {
	std::srand((unsigned int) time(nullptr));
}

RandomSound::RandomSound(vector<Sound *> sounds)
	: sounds(sounds) {
	std::srand((unsigned int) time(nullptr));
};

void RandomSound::addSound(Sound * sound) {
	sounds.push_back(sound);
}

void RandomSound::playSound() {
	if (lastPlayed && !lastPlayed->isFinished()) return;

	size_t numSounds = sounds.size();
	int randIndex = std::rand() % numSounds;
	sounds[randIndex]->play(false);
	lastPlayed = sounds[randIndex];
}