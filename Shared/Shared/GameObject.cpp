#include "GameObject.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

GameObject::GameObject() {
	GameObject(vec3(0, 0, 0), vec3(0, 0, 0), 0);
}

GameObject::GameObject(const int &id): GameObject() {
	this->id = id;
}

GameObject::GameObject(vec3 position, vec3 velocity, int id)
#ifdef _SERVER
	: model(""), material(""), animation(-1)
#endif
{
	this->position = position;
	this->velocity = velocity;
	this->id = id;
	setBoundingShape(nullptr);
}
  
BoundingShape * GameObject::getBoundingShape() {
	return this->boundingShape;
}

void GameObject::setBoundingShape(BoundingShape * boundingShape) {
	if (getBoundingShape()) {
		delete this->boundingShape;
	}
	this->boundingShape = boundingShape;
}

vec3 GameObject::setPosition(vec3 pos) {
	this->prevPosition = this->position;
	this->position = pos;
	this->boundingShape->setPosition(pos);
	return prevPosition;
}

vec3 GameObject::getPosition() {
	return this->position;
}

vec3 GameObject::getPrevPosition() {
	return this->prevPosition;
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

GAMEOBJECT_TYPES GameObject::getGameObjectType() const {
	return GAMEOBJECT_TYPES::GAMEOBJECT_TYPE;
}

void GameObject::setId(int id) {
	this->id = id;
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

bool GameObject::collidesWith(GameObject * gameObject) {
	return this->getBoundingShape()->collideVisit(gameObject->getBoundingShape());
}

void GameObject::onCollision(GameObject * gameObject) {
	// std::cout << "not implemented" << std::endl;
}

void GameObject::onCollision(Ball * ball) { 
	// std::cout << "not implemented" << std::endl; 
}

void GameObject::onCollision(Bullet * bullet) {
}

void GameObject::onCollision(Goal * goal) {
}

void GameObject::onCollision(Paddle * paddle) { 
	// std::cout << "not implemented" << std::endl; 
}

void GameObject::onCollision(Player * player) {
	// std::cout << "not implemented" << std::endl;
}

void GameObject::onCollision(Wall * wall) { 
	// std::cout << "not implemented" << std::endl; 
}

double GameObject::distanceFrom(GameObject * gameObject) {
	return glm::distance(position, gameObject->getPosition());
}

bool GameObject::deleteOnServerTick() {
	return false;
}

void GameObject::updateOnServerTick() {
	return;
}

void GameObject::setOrientation(const quat &newOrientation) {
	orientation = newOrientation;
}

const quat &GameObject::getOrientation() const {
	return orientation;
}

void GameObject::serialize(NetBuffer &buffer) const {
	buffer.write<quat>(orientation);
	buffer.write<vec3>(position);
	buffer.write<vec3>(velocity);
	buffer.write<vec3>(scale);
	buffer.write<int>(id);
	// NOT SURE NEED TO WRITE BOUNDING SHAPE TO NETBUFFER
	// SHOULD ONLY BE SERVER SIDE, PLEASE CONFIRM IN CODE REVIEW
	// buffer.write<int>(radius);
}

void GameObject::deserialize(NetBuffer &buffer) {
	orientation = buffer.read<quat>();
	position = buffer.read<vec3>();
	velocity = buffer.read<vec3>();
	scale = buffer.read<vec3>();
	id = buffer.read<int>();
	// radius = buffer.read<int>();
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
	case BULLET_TYPE:
		type = "bullet";
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