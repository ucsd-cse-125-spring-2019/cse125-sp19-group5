#include "RandomSound.h"

RandomSound::RandomSound() {
	std::srand(time(nullptr));
}

RandomSound::RandomSound(vector<Sound> sounds)
	: sounds(sounds) {
	std::srand(time(nullptr));
};

void RandomSound::addSound(Sound sound) {
	sounds.push_back(sound);
}

void RandomSound::playSound() {
	int numSounds = sounds.size();
	int randIndex = std::rand() % numSounds;
	sounds[randIndex].play(false);
}