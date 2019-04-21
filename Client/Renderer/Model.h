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
	std::vector<Mesh*> meshes;
	std::vector<mat4> boneTransforms;
	int curAnimation = -1;
	float animStartTime = 0.0;

	// The model is imported in a tree hierchy. This is used to recursively
	// traverse the tree and convert the nodes into a Mesh instance.
	void loadNode(aiNode *node, const aiScene *scene);

	public:
	// Creates a model given the path to the model file.
	Model(const std::string &path);
	~Model();

	// Draws the model with the given shader.
	void draw(Shader &shader) const;

	// Sets the current animation for the meshes of this model.
	void setAnimation(int id = -1, bool restart = true);

	// Continues the animations for the meshes of the model using the current
	// time in seconds.
	void updateAnimation(float time);
};

