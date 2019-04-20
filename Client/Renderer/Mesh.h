#pragma once

#include <GL/glew.h>
#include <vector>
#include <Shared/Common.h>
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>

// The maximum number of bones that can affect a vertex.
constexpr auto BONES_PER_VERTEX = 4;

// Type of number used for indices in the element buffer.
typedef unsigned int ElementIndex;

// Container for the position vector and normal vector of a vertex.
struct Vertex {
	vec3 pos;
	vec3 normal;
	vec2 texCoords;
	GLuint bones[BONES_PER_VERTEX] = { 0 };
	GLfloat weights[BONES_PER_VERTEX] = { 0.0f };
};

struct Bone {
	std::string name;
	mat4 offset;
};

// The Mesh class is used to draw a collection of vertices using triangles in the
// order given by indices.
class Mesh {
	friend class Model;

	private:
	GLuint VAO, VBO, EBO;
	GLsizei numVertices;
	std::vector<Bone> bones;
	std::unordered_map<std::string, int> boneIds;
	std::unordered_map<const char*, aiNodeAnim> nodeAnims;
	std::vector<mat4> boneTransformations;
	aiMesh *mesh;
	const aiScene *scene;
	int animationId = -1;

	// Gets the vertices of the aiMesh into a list of Vertex instances.
	static std::vector<Vertex> loadVertices(aiMesh *mesh);

	// Gets the indices for the vertices into a list of numbers.
	static std::vector<ElementIndex> loadIndices(aiMesh *mesh);

	// Sets up which bones will affect which vertices for the model.
	void loadBones(std::vector<Vertex> &vertices);

	// Sets up the vertex transformations for the current animation.
	void buildBoneTransformations(float time, aiNode *node, const mat4 &parentTransform);
	aiNodeAnim *getNodeAnim(const aiAnimation *animation, const std::string &nodeName);

	public:
	Mesh(aiMesh *mesh, const aiScene *scene);
	~Mesh();

	void updateAnimation(float time);

	void draw(Shader &shader) const;
};

