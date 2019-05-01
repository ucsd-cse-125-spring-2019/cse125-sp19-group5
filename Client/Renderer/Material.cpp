#include "Material.h"

#define SET_MATERIAL_PARAM(param) shader.setUniform("material." #param, ##param)
#define SET_MATERIAL_TEX_PARAM(position, tex) if (##tex) { \
	##tex->bind(position); \
	shader.setUniform("material." #tex, position); \
}

Material::Material(const std::string &path) {
	static const auto WHITE = new Texture2d("Textures/white.png");
	static const auto BLACK = new Texture2d("Textures/black.png");

	diffuseTex = WHITE;
	specularTex = WHITE;
	emissionTex = BLACK;
}

void Material::bind(Shader &shader) const {
	SET_MATERIAL_PARAM(shininess);

	SET_MATERIAL_TEX_PARAM(0, diffuseTex);

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
