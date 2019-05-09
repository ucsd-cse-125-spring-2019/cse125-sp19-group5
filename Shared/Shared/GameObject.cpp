#include "GameObject.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

GameObject::GameObject(): GameObject(vec3(0.0f), vec3(0.0f), 0, 1) { }

GameObject::GameObject(const int &id): GameObject() {
	this->id = id;
}

GameObject::GameObject(vec3 position, vec3 velocity, int id, int radius)
#ifdef _SERVER
	: model(""), material(""), animation(-1)
#endif
{
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
	//std::cout << to_string() << " collided with " << gameObject->to_string() << std::endl;
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

void GameObject::setScale(const vec3 &newScale) {
	scale = newScale;
}

vec3 GameObject::getScale() const {
	return scale;
}

int GameObject::getRadius() {
	return this->radius;
}

int GameObject::getId() {
	return this->id;
}

vec3 GameObject::getMoveDestination(vec3 movement) {
	return getPosition() + movement;
}

void GameObject::move(vec3 movement) {
	setPosition(getMoveDestination(movement));
}

bool GameObject::deleteOnServerTick() {
	return false;
}

void GameObject::updateOnServerTick() {
	return;
}

void GameObject::serialize(NetBuffer &buffer) const {
	buffer.write<vec3>(position);
	buffer.write<vec3>(velocity);
	buffer.write<vec3>(scale);
	buffer.write<int>(id);
	buffer.write<int>(radius);
}

void GameObject::deserialize(NetBuffer &buffer) {
	position = buffer.read<vec3>();
	velocity = buffer.read<vec3>();
	scale = buffer.read<vec3>();
	id = buffer.read<int>();
	radius = buffer.read<int>();
}

string GameObject::to_string() {
	string type;
	switch(getGameObjectType()) {
	case PLAYER_TYPE:
		type = "player";
		break;
	case BALL_TYPE:
		type = "ball";
		break;
	case WALL_TYPE:
		type = "wall";
		break;
	case PADDLE_TYPE:
		type = "paddle";
		break;
	default:
		"gameobject";
		break;
	}

	return type + "_" + std::to_string(getId()) + glm::to_string(getPosition()).substr(4);
}