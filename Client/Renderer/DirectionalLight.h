#pragma once

#include "Light.h"

// A DirectionalLight is a light that seems very far away (at infinity), used for
// lights like sunlight.
class DirectionalLight : public Light {
	using Light::Light;

	private:
	vec3 direction = -vec3(0.0f, -1.0f, 0.0f);
	vec3 ambient = vec3(0.0f);

	protected:
	void bindValues(Shader &shader) const;
	std::string getType() const;

	public:
	void setDirection(const vec3 &newDirection);
	void setAmbient(const vec3 &newAmbient);
	vec3 getDirection() const;
	vec3 getAmbient() const;
};
