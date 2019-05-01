#pragma once

#include <GL/glew.h>
#include <vector>
#include <Shared/Common.h>
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include "MeshData.h"

// The Mesh class is used to draw a collection of vertices using triangles in the
// order given by indices.
class Mesh {
	friend class Model;

	private:
	std::vector<mat4> boneTransformations;
	MeshData *data;
	int animationId = -1;

	// Sets up the vertex transformations for the current animation.
	void buildBoneTransformations(float time, aiNode *node, const mat4 &parentTransform);

	public:
	Mesh(MeshData *data);

	void updateAnimation(float time);

	void draw(Shader &shader) const;
};

