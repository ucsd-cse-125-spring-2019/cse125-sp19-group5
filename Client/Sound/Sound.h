#pragma once

#include <Shared/Common.h>
#include <irrKlang/irrKlang.h>

class Sound
{
public:
	Sound();
	Sound(std::string filepath, irrklang::ISound &sound);
	~Sound();
	void play(const bool isLooping);
	void pause();
	void setPosition(vec3 pos);
	void update();

private:
	std::string filepath;
	irrklang::ISound * sound = nullptr;
	bool isSpatial = false;
	irrklang::vec3df position;
	irrklang::vec3df velocity;
};