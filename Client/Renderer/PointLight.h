#pragma once

#include "Light.h"

// A PointLight is a light at a certain position and its brightness falls off.
class PointLight : public Light {
	using Light::Light;

	private:
	float constant = 0.1f;
	float linear = 1.0f;
	float quadratic = 0.2f;
	vec3 position = vec3(0.0f);

	protected:
	void bindValues(Shader &shader) const;

	public:
	void setPosition(const vec3 &newPosition);
	void setConstant(float value);
	void setLinear(float value);
	void setQuadratic(float value);
	std::string getType() const;
};

