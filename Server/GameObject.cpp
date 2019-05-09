#include <Shared/GameObject.h>
#include "Networking/Server.h"

void GameObject::setModel(const string &newModel) {
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