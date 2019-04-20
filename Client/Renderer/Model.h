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
	Assimp::Importer importer;
	const aiScene *scene;
	std::vector<Mesh> meshes;
	std::vector<mat4> boneTransforms;
	int curAnimation = -1;

	// The model is imported in a tree hierchy. This is used to recursively
	// traverse the tree and convert the nodes into a Mesh instance.
	void loadNode(aiNode *node, const aiScene *scene);

	// Converts the Assimp representation of a mesh to a Mesh instance.
	Mesh loadMesh(aiMesh *mesh, const aiScene *scene);

	// Converts the vertices from the aiMesh to a list of Vertex instances.
	std::vector<Vertex> loadMeshVertices(aiMesh *mesh);

	// Gets a list of the vertex indices for an aiMesh.
	std::vector<ElementIndex> loadMeshIndices(aiMesh *mesh);

	// Loads the bones for a mesh.
	void loadMeshBones(aiMesh *mesh, std::vector<Vertex> &vertices);

	public:
	Model(const std::string &path);
	void draw(Shader &shader) const;

	void updateAnimation(float time);
};

