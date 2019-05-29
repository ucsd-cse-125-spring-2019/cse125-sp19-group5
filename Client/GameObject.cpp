#include <Shared/GameObject.h>

#define DO_NOT_CALL throw std::runtime_error(__FUNCTION__ " should not be called")

void GameObject::setModel(const std::string &newModel) { DO_NOT_CALL; }
void GameObject::setMaterial(const std::string &newModel) { DO_NOT_CALL; }
void GameObject::setAnimation(int newAnimation, bool reset) { DO_NOT_CALL; }

std::string GameObject::getModel() const { DO_NOT_CALL; return ""; }
std::string GameObject::getMaterial() const { DO_NOT_CALL; return ""; }
int GameObject::getAnimation() const { DO_NOT_CALL; return -1; }

void GameObject::playSound(const string &sound, float volume, bool loop) {
	DO_NOT_CALL;
}
void GameObject::stopSound(const string &sound) { DO_NOT_CALL; }