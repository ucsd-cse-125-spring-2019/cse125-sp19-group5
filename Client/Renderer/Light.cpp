#include "Light.h"

Light::Light(int index): index(index), color(vec3(1.0f)) { }

void Light::bind(Shader &shader) const {
	SET_LIGHT_UNIFORM(color);
	bindValues(shader);
}

void Light::setColor(const vec3 &newColor) {
	color = newColor;
}

std::string Light::getType() const {
	return "light";
}
