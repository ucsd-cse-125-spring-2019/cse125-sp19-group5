#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MeshData.h"

class ModelData {
	private:
	Assimp::Importer importer;
	const aiScene *scene;
	std::vector<MeshData*> meshes;

	// The model is imported in a tree hierchy. This is used to recursively
	// traverse the tree and convert the nodes into a Mesh instance.
	void loadNode(aiNode *node, const aiScene *scene);

	public:
	// Creates a model given the path to the model file.
	ModelData(const std::string &path);
	const std::vector<MeshData*> &getMeshData() const;
	~ModelData();
};