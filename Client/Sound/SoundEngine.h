#pragma once

#include <Shared/Common.h>
#include <irrKlang/irrKlang.h>

#include "Sound.h"

class SoundEngine
{
public:
	SoundEngine();
	~SoundEngine();

	bool isInitialized();
	Sound *loadFlatSound(const std::string &filepath, float volume);
	Sound *loadSpatialSound(const std::string &filepath, float volume);
	void setMasterVolume(float volume);
	void update(vec3 pos, vec3 vel, vec3 lookDir);

private:
	irrklang::ISoundEngine * engine;
	bool didInitialize = false;
};

extern SoundEngine *gSound;