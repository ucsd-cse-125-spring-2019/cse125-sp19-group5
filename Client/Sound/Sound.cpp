#include "Sound.h"

Sound::Sound() {
}

Sound::Sound(std::string filepath, irrklang::ISound &sound)
	: filepath(filepath)
	, sound(&sound) {
}

Sound::~Sound() {
	sound->drop();
	sound = nullptr;
}

void Sound::update() {

}

void Sound::play(const bool isLooping) {
	sound->setIsLooped(isLooping);
	sound->setIsPaused(false);
}

void Sound::pause() {
	sound->setIsPaused(true);
}

void Sound::setPosition(vec3 pos) {
	irrklang::vec3df soundPos = irrklang::vec3df(pos.x, pos.y, pos.z);
	position = soundPos;
	isSpatial = true;
}