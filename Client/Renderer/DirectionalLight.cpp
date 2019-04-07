#include "DirectionalLight.h"

void DirectionalLight::bindValues(Shader &shader) const {
	SET_LIGHT_UNIFORM(direction);
}

std::string DirectionalLight::getType() const {
	return "directionalLight";
}

void DirectionalLight::setDirection(const vec3 &newDirection) {
	direction = glm::normalize(newDirection);
}
