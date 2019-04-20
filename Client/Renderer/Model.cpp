#include "Model.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Model::Model(const std::string &path) {
	// Create a scene from the given path to the model file.
	scene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals
	);

	// Report errors if any.
	if (
		!scene ||
		scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode
	) {
		std::cerr << "Model(" << path << ") failed to load!" << std::endl
			<< importer.GetErrorString() << std::endl;
		return;
	}

	// If no errors, then create the meshes for the model.
	loadNode(scene->mRootNode, scene);
}

void Model::draw(Shader &shader) const {
	for (auto mesh : meshes) {
		mesh.draw(shader);
	}
}

void Model::loadNode(aiNode *node, const aiScene *scene) {
	// Convert the current node to meshes.
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(Mesh(mesh, scene));
	}

	// Then, recursively load the node's children.
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		loadNode(node->mChildren[i], scene);
	}
}

void Model::updateAnimation(float time) {
	for (auto &mesh : meshes) {
		mesh.updateAnimation(time);
	}
}