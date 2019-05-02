#pragma once

#include <Shared/Common.h>
#include <irrKlang/irrKlang.h>

class Sound
{
public:
	Sound();
	Sound(irrklang::ISound &sound, float volume, const bool isSpatial);
	~Sound();
	void play(const bool isLooping);
	void pause();
	void setVolume(float volume);
	void setPosition(vec3 pos);
	void setVelocity(vec3 vel);
	void setMinDist(float dist);
	void setMaxDist(float dist);

private:
	std::string filepath;
	irrklang::ISound * sound = nullptr;
	bool isSpatial = false;
};