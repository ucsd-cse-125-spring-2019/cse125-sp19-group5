#include "GameObject.h"

bool GameObject::collidesWith(GameObject &gameObject) {
	return false;
}

void GameObject::onCollision(GameObject &gameObject) {

}

double GameObject::distanceFrom(GameObject &gameObject) {
	return 2;
}
