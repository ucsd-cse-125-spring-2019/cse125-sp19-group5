#pragma once

#include <GL/glew.h>
#include <Shared/Common.h>
#include <string>
#include "Texture2d.h"
#include "Shader.h"

// A Material contains parameters that determine how lighting for a
// 3D model is done.
class Material {
	private:
	float shininess = 32.0f;
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(1.0f);
	vec3 emission = vec3(0.0f);
	Texture2d *diffuseTex = nullptr;
	Texture2d *normalTex = nullptr;
	Texture2d *specularTex = nullptr;
	Texture2d *emissionTex = nullptr;

	public:
	Material();
	Material(const std::string &path);

	// Setters for material parameters.
	void setShininess(float newShininess);
	void setDiffuseTexture(Texture2d *newDiffuseTex);
	void setNormalTexture(Texture2d *newNormalTex);
	void setSpecularTexture(Texture2d *newSpecularTex);
	void setEmissionTexture(Texture2d *newEmissionTex);


	// Getters for material parameters.
	float getShininess() const;
	Texture2d *getDiffuseTexture() const;
	Texture2d *getNormalTexture() const;
	Texture2d *getSpecularTexture() const;
	Texture2d *getEmissionTexture() const;

	// Set material uniforms for the given shader.
	void bind(Shader &shader) const;
};