#pragma once

#include <string>
#include <Shared/Common.h>
#include "Shader.h"

#define SET_LIGHT_UNIFORM(value) (shader.setUniform((getType() + "[" + std::to_string(index) + "]." + #value).c_str(), (value)))

class Light
{
	protected:
	vec3 color; // Color of the light.
	int index; // Index into the array for the type of light.

	// Allow subclasses to set their own uniforms when binding the light.
	virtual void bindValues(Shader &shader) const = 0;

	public:
	explicit Light(int index);

	// Sets the color of the light.
	void setColor(const vec3 &newDiffuse);

	// Sets the uniform values corresponding to this light.
	void bind(Shader &shader) const;

	// Returns the name of the array for the light uniforms.
	virtual std::string getType() const;
};

