#include "Model.h"
#include "ModelData.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "../Assets.h"

Model::Model(const std::string &path) {
	auto modelData = Assets::getModelData(path);
	for (auto meshData : modelData->getMeshData()) {
		meshes.push_back(new Mesh(meshData));
	}
}

void Model::draw(Shader &shader) const {
	for (auto mesh : meshes) {
		mesh->draw(shader);
	}
}

void Model::setAnimation(int id, bool restart) {
	curAnimation = id;
	if (restart) {
		animStartTime = (float)glfwGetTime();
	}
	for (auto mesh : meshes) {
		mesh->animationId = id;
	}
}

void Model::updateAnimation(float time) {
	for (auto mesh : meshes) {
		mesh->updateAnimation(time - animStartTime);
	}
}

Model::~Model() {
	for (auto mesh : meshes) {
		delete mesh;
	}
}