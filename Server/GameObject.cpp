#include <Shared/GameObject.h>
#include "Networking/Server.h"

void GameObject::setModel(const string &newModel) {
	model = newModel;
	NetBuffer buffer(NetMessage::GAME_OBJ_MODEL);
	buffer.write<int>(getId());
	buffer.write<std::string>(newModel);
	Network::broadcast(buffer);
}

string GameObject::getModel() const {
	return model;
}