#include "DirectionalLight.h"

void DirectionalLight::bindValues(Shader &shader) const {
	SET_LIGHT_UNIFORM(direction);
	SET_LIGHT_UNIFORM(ambient);
}

std::string DirectionalLight::getType() const {
	return "directionalLight";
}

void DirectionalLight::setDirection(const vec3 &newDirection) {
	direction = glm::normalize(-newDirection);
}

void DirectionalLight::setAmbient(const vec3 &newAmbient) {
	ambient = newAmbient;
}

vec3 DirectionalLight::getDirection() const {
	return direction;
}

vec3 DirectionalLight::getAmbient() const {
	return ambient;
}
