#include <Shared/GameObject.h>
#include "Networking/Server.h"

void GameObject::setModel(const std::string &newModel) {
	model = newModel;
	NetBuffer buffer(NetMessage::GAME_OBJ_MODEL);
	buffer.write(getId());
	buffer.write(newModel);
	Network::broadcast(buffer);
}

string GameObject::getModel() const {
	return model;
}

void GameObject::setAnimation(int id, bool reset) {
	animation = id;
	NetBuffer buffer(NetMessage::GAME_OBJ_ANIM);
	buffer.write(getId());
	buffer.write(id);
	buffer.write(reset);
	Network::broadcast(buffer);
}

int GameObject::getAnimation() const {
	return animation;
}

void GameObject::setMaterial(const std::string &newMaterial) {
	material = newMaterial;
	NetBuffer buffer(NetMessage::GAME_OBJ_MAT);
	buffer.write(getId());
	buffer.write(newMaterial);
	Network::broadcast(buffer);
}

string GameObject::getMaterial() const {
	return material;
}

void GameObject::playSound(const string &sound, float volume, bool loop) {
	NetBuffer buffer(NetMessage::SOUND);
	buffer.write(id);
	buffer.write(sound);
	buffer.write(volume);
	buffer.write(loop);
	Network::broadcast(buffer);
}

void GameObject::stopSound(const string &sound) {
	playSound(sound, -1.0f, false);
}