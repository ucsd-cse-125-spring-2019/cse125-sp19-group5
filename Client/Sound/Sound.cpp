#include "Sound.h"

Sound::Sound(): sound(nullptr) {
}

Sound::Sound(std::string filepath, irrklang::ISound &sound)
	: filepath(filepath)
	, sound(&sound) {
}

Sound::~Sound() {
	if (sound) {
		sound->drop();
		sound = nullptr;
	}
}

void Sound::update() {

}

void Sound::play(const bool isLooping) {
	if (!sound) { return; }
	sound->setIsPaused(false);
	sound->setIsLooped(isLooping);
}

void Sound::pause() {
	if (!sound) { return; }
	sound->setIsPaused(true);
}

void Sound::setPosition(vec3 pos) {
	if (!sound) { return; }
	irrklang::vec3df soundPos = irrklang::vec3df(pos.x, pos.y, pos.z);
	position = soundPos;
	isSpatial = true;
}