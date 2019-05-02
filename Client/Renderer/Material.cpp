#include "Material.h"
#include <fstream>
#include <iostream>
#include <Shared/Util/json11.hpp>

#define SET_MATERIAL_PARAM(param) shader.setUniform("material." #param, ##param)
#define SET_MATERIAL_TEX_PARAM(position, tex) if (##tex) { \
	##tex->bind(position); \
	shader.setUniform("material." #tex, position); \
}

Material::Material() {
	static const auto WHITE = new Texture2d("Textures/white.png");
	static const auto BLACK = new Texture2d("Textures/black.png");

	diffuseTex = WHITE;
	specularTex = WHITE;
	emissionTex = BLACK;
}

void loadTextureFromConfig(
	const std::string &key,
	json11::Json &info,
	Texture2d *&tex
) {
	auto value = info[key].string_value();
	if (value != "") {
		// TODO (bhang): use some resource cache for textures.
		tex = new Texture2d(info[key].string_value());
	}
}

Material::Material(const std::string &path): Material() {
	std::ifstream f(path);

	if (!f.is_open()) {
		std::cerr << "Failed to open material (" << path << ")" << std::endl;
		return;
	}

	std::string source(
		(std::istreambuf_iterator<char>(f)),
		std::istreambuf_iterator<char>()
	);

	std::string parseErr = "";
	auto info = json11::Json::parse(source, parseErr);

	if (parseErr != "") {
		std::cerr << "Failed to parse material info (" << path << ")"
			<< std::endl << parseErr << std::endl;
		return;
	}

	loadTextureFromConfig("diffuse", info, diffuseTex);
	loadTextureFromConfig("normal", info, normalTex);
	loadTextureFromConfig("specular", info, specularTex);
	loadTextureFromConfig("emission", info, emissionTex);

	if (info["shininess"].is_number()) {
		shininess = (float)info["shininess"].number_value();
	}
}

void Material::bind(Shader &shader) const {
	// Set shininess float.
	SET_MATERIAL_PARAM(shininess);

	// Then, bind the diffuse, specular, emission, and normal maps.
	SET_MATERIAL_TEX_PARAM(0, diffuseTex);

	// If no normal map is set, fall back to the mesh's original normals.
	shader.setUniform("material.hasNormalMap", normalTex != nullptr);
	if (normalTex) {
		SET_MATERIAL_TEX_PARAM(1, normalTex);
	}

	SET_MATERIAL_TEX_PARAM(2, specularTex);
	SET_MATERIAL_TEX_PARAM(3, emissionTex);
}

void Material::setShininess(float newShiniess) {
	shininess = newShiniess;
}

void Material::setDiffuseTexture(Texture2d *newDiffuseTex) {
	diffuseTex = newDiffuseTex;
}

void Material::setNormalTexture(Texture2d *newNormalTex) {
	normalTex = newNormalTex;
}

void Material::setSpecularTexture(Texture2d *newSpecularTex) {
	specularTex = newSpecularTex;
}

void Material::setEmissionTexture(Texture2d *newEmissionTex) {
	emissionTex = newEmissionTex;
}

float Material::getShininess() const {
	return shininess;
}

Texture2d* Material::getDiffuseTexture() const {
	return diffuseTex;
}

Texture2d* Material::getNormalTexture() const {
	return normalTex;
}

Texture2d* Material::getSpecularTexture() const {
	return specularTex;
}

Texture2d* Material::getEmissionTexture() const {
	return emissionTex;
}
