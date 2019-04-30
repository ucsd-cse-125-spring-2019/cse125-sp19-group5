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
	Sound *loadFlatSound(const std::string &filepath);
	Sound *loadSpatialSound(const std::string &filepath, const vec3 pos);

private:
	irrklang::ISoundEngine * engine;
	bool didInitialize = false;
};

