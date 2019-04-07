#include "PointLight.h"

void PointLight::bindValues(Shader &shader) const {
	SET_LIGHT_UNIFORM(position);
	SET_LIGHT_UNIFORM(constant);
	SET_LIGHT_UNIFORM(linear);
	SET_LIGHT_UNIFORM(quadratic);
}

void PointLight::setPosition(const vec3 &newPosition) {
	position = newPosition;
}

void PointLight::setConstant(float value) {
	constant = value;
}

void PointLight::setLinear(float value) {
	linear = value;
}

void PointLight::setQuadratic(float value) {
	quadratic = value;
}

std::string PointLight::getType() const {
	return "pointLight";
}
