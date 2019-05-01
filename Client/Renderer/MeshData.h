#pragma once

#include <GL/glew.h>
#include <Shared/Common.h>
#include <vector>
#include <unordered_map>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

using std::vector;
using std::unordered_map;

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

inline mat4 toGlm(const aiMatrix4x4 &m) {
	return glm::transpose(glm::make_mat4(&m.a1));
}

class MeshData {
	private:
	GLuint VAO, VBO, EBO;
	GLsizei numVertices;
	vector<Bone> bones;
	unordered_map<std::string, int> boneIds;
	unordered_map<const char*, aiNodeAnim> nodeAnims;
	aiMesh *mesh;
	const aiScene *scene;

	// Gets the vertices of the aiMesh into a list of Vertex instances.
	static std::vector<Vertex> loadVertices(aiMesh *mesh);

	// Gets the indices for the vertices into a list of numbers.
	static std::vector<ElementIndex> loadIndices(aiMesh *mesh);

	// Sets up which bones will affect which vertices for the model.
	void loadBones(std::vector<Vertex> &vertices);


	public:
	MeshData(aiMesh *mesh, const aiScene *scene);
	~MeshData();

	aiNodeAnim *getNodeAnim(const aiAnimation *animation, const std::string &nodeName);
	const unordered_map<std::string, int> &getBoneIds() const;
	const vector<Bone> &getBones() const;
	const int getNumAnimations() const;
	const aiAnimation *getAnimation(int index) const;
	aiNode *getSceneRoot() const;
	void draw(Shader &shader) const;
};
