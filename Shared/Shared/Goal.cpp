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

void Goal::setBallSpawnRange(tuple<float, float> xRange, tuple<float, float> yRange, tuple<float, float> zRange) {
	this->xRange = xRange;
	this->yRange = yRange;
	this->zRange = zRange;
}

vec3 Goal::getRandomSpawnPos() {
	float xPos = (rand() / (float)RAND_MAX * (std::get<1>(xRange) - std::get<0>(xRange))) + std::get<0>(xRange);
	float yPos = (rand() / (float)RAND_MAX * (std::get<1>(yRange) - std::get<0>(yRange))) + std::get<0>(yRange);
	float zPos = (rand() / (float)RAND_MAX * (std::get<1>(zRange) - std::get<0>(zRange))) + std::get<0>(zRange);

	return vec3(xPos, yPos, zPos);
}

void Goal::onCollision(GameObject * gameObject) {
	gameObject->onCollision(this);
}

void Goal::onCollision(Ball * ball) {
	this->goalsScored += 1;
}