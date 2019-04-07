#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

// The drawable 3D model. The model is specified by a file.
class Model
{
	private:
	std::vector<Mesh> meshes;

	// The model is imported in a tree hierchy. This is used to recursively
	// traverse the tree and convert the nodes into a Mesh instance.
	void loadNode(aiNode *node, const aiScene *scene);

	// Converts the Assimp representation of a mesh to a Mesh instance.
	Mesh loadMesh(aiMesh *mesh, const aiScene *scene);

	public:
	Model(const std::string &path);
	void draw(const Shader &shader) const;
};

