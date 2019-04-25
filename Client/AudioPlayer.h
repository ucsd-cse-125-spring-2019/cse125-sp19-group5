#pragma once

#include <Shared/Common.h>
#include <irrKlang/irrKlang.h>

class AudioPlayer
{
public:
	AudioPlayer();
	~AudioPlayer();

	bool isInitialized();
	void playLoop(const std::string &filepath);

private:
	irrklang::ISoundEngine * engine;
	bool didInitialize = false;
};

