#include "GameObject.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

GameObject::GameObject() {
	GameObject(vec3(0, 0, 0), vec3(0, 0, 0), "default", 1);
}

GameObject::GameObject(vec3 position, vec3 velocity, string id, int radius) {
	this->position = position;
	this->velocity = velocity;
	this->id = id;
	this->radius = radius;
}

bool GameObject::collidesWith(GameObject * gameObject) {
	if (this == gameObject) {
		return false;
	}
	return distanceFrom(gameObject) < (radius + gameObject->radius);
}

void GameObject::onCollision(GameObject * gameObject) {
	// Should only update your own state, gameObject's state will be updated when it calls onCollision
	std::cout << "GameObject: " + id + glm::to_string(position) + " collided with " + gameObject->id + glm::to_string(gameObject->position) << std::endl;
}

double GameObject::distanceFrom(GameObject * gameObject) {
	return glm::distance(position, gameObject->getPosition());
}

vec3 GameObject::setPosition(vec3 pos) {
	vec3 oldPosition = this->position;
	this->position = pos;
	return oldPosition;
}

vec3 GameObject::getPosition() {
	return this->position;
}

vec3 GameObject::setVelocity(vec3 velocity) {
	vec3 oldVelocity = this->velocity;
	this->velocity = velocity;
	return oldVelocity;
}

vec3 GameObject::getVelocity() {
	return this->velocity;
}

string GameObject::getId() {
	return this->id;
}

string GameObject::getGameObjectType() {
	return id.substr(0, id.find("_"));
}

int GameObject::getIntId() {
	return stoi(id.substr(id.find("_") + 1));
}

void GameObject::move(vec3 movement) {
	setPosition(getPosition() + movement);
}

bool GameObject::deleteOnServerTick() {
	return false;
}

void GameObject::updateOnServerTick() {
	return;
}