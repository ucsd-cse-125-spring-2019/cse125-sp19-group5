#include "GameObject.h"

bool GameObject::collidesWith(GameObject &gameObject) {
	return distanceFrom(gameObject) < (radius + gameObject.radius);
}

void GameObject::onCollision(GameObject &gameObject) {
}

double GameObject::distanceFrom(GameObject &gameObject) {
	return glm::distance(position, gameObject.position);
}
