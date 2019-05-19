#include "Goal.h"

GAMEOBJECT_TYPES Goal::getGameObjectType() const {
	return GOAL_TYPE;
}

void Goal::setTeam(int team) {
	this->team = team;
}

int & Goal::getTeam() {
	return this->team;
}

int & Goal::getGoalsScored() {
	return this->goalsScored;
}

void Goal::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Goal::onCollision(Ball * ball) {
	this->goalsScored += 1;
}