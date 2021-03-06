#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

// The drawable 3D model. The model is specified by a file.
class Model {
	private:
	std::vector<Mesh*> meshes;
	int curAnimation = -1;
	float animStartTime = 0.0;

	public:
	// Creates a model given the path to the model file.
	Model(const std::string &path);

	// Draws the model with the given shader.
	void draw(Shader &shader) const;

	// Sets the current animation for the meshes of this model.
	void setAnimation(int id = -1, bool restart = true);

	// Continues the animations for the meshes of the model using the current
	// time in seconds.
	void updateAnimation(float time);

	~Model();
};

