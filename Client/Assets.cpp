#include "Assets.h"

unordered_map<string, Texture2d*> Assets::textures;
unordered_map<string, ModelData*> Assets::models;
unordered_map<string, Shader*> Assets::shaders;

ModelData *Assets::getModelData(const string &path) {
	if (models.find(path) == models.end()) {
		models[path] = new ModelData(path);
	}
	return models[path];
}

Texture2d *Assets::getTexture2d(const string &path) {
	if (textures.find(path) == textures.end()) {
		textures[path] = new Texture2d(path);
	}
	return textures[path];
}

Shader *Assets::getShader(const string &path) {
	if (shaders.find(path) == shaders.end()) {
		shaders[path] = new Shader(path);
	}
	return shaders[path];
}

void Assets::cleanUp() {
	for (auto texture : textures) {
		delete texture.second;
	}
	for (auto model : models) {
		delete model.second;
	}
}
