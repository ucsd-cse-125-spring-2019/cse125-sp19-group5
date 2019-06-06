#pragma once

#include "Sound.h"
#include <ctime>

class RandomSound
{
public:
	RandomSound();
	RandomSound(vector<Sound> sounds);

	void addSound(Sound sound);
	void playSound();

private:
	vector<Sound> sounds;
};
