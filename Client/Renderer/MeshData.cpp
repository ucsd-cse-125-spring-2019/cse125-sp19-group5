#include "MeshData.h"

MeshData::MeshData(aiMesh *mesh, const aiScene *scene): mesh(mesh), scene(scene) {
	auto vertices = loadVertices(mesh);
	auto indices = loadIndices(mesh);
	loadBones(vertices);

	VAO = 0;
	numVertices = static_cast<GLsizei>(indices.size());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Set up the VBO for the mesh
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW
	);

	// Set up the EBO for the mesh
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(ElementIndex),
		indices.data(),
		GL_STATIC_DRAW
	);

	// Location 0: Vertex Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(0);

	// Location 1: Vertex Normal
	auto offset = (const GLvoid*)offsetof(Vertex, normal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset);
	glEnableVertexAttribArray(1);

	// Location 2: Texture Coordinates
	offset = (const GLvoid*)offsetof(Vertex, texCoords);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset);
	glEnableVertexAttribArray(2);

	// Location 3: Bone Indices for Weights
	offset = (const GLvoid*)offsetof(Vertex, bones);
	glVertexAttribIPointer(3, BONES_PER_VERTEX, GL_INT, sizeof(Vertex), offset);
	glEnableVertexAttribArray(3);

	// Location 4: Bone Weights
	offset = (const GLvoid*)offsetof(Vertex, weights);
	glVertexAttribPointer(4, BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset);
	glEnableVertexAttribArray(4);

	// Clean up
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::vector<Vertex> MeshData::loadVertices(aiMesh *mesh) {
	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		} else {
			vertex.normal = vec3(0.0f, 0.0f, 1.0f);
		}

		if (mesh->mTextureCoords[0]) {
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		} else {
			vertex.texCoords = vec2(0.0f);
		}

		vertices.push_back(vertex);
	}
	return vertices;
}

std::vector<ElementIndex> MeshData::loadIndices(aiMesh *mesh) {
	std::vector<ElementIndex> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	return indices;
}

void MeshData::loadBones(std::vector<Vertex> &vertices) {
	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		auto boneInfo = mesh->mBones[i];
		auto boneIndex = 0;

		// Get the unique ID for the bone by name.
		std::string boneName(boneInfo->mName.data);
		auto boneIdIt = boneIds.find(boneName);
		if (boneIdIt == boneIds.end()) {
			boneIndex = boneIds[boneName] = (int)bones.size();

			Bone bone;
			bone.name = boneName;
			bone.offset = toGlm(boneInfo->mOffsetMatrix);

			bones.push_back(bone);
		} else {
			boneIndex = boneIdIt->second;
		}

		for (unsigned int j = 0; j < boneInfo->mNumWeights; j++) {
			auto vertexWeight = boneInfo->mWeights[j];
			auto vertexId = vertexWeight.mVertexId;

			for (int freeIndex = 0; freeIndex < BONES_PER_VERTEX; freeIndex++) {
				if (vertices[vertexId].weights[freeIndex] != 0.0f) {
					continue;
				}
				vertices[vertexId].bones[freeIndex] = boneIndex;
				vertices[vertexId].weights[freeIndex] = vertexWeight.mWeight;
			}
		}
	}
}

aiNodeAnim *MeshData::getNodeAnim(const aiAnimation *animation, const std::string &nodeName) {
	const auto nodeNameCStr = nodeName.c_str();
	const auto size = nodeName.size();

	for (unsigned int i = 0; i < animation->mNumChannels; i++) {
		auto nodeAnim = animation->mChannels[i];
		if (strncmp(nodeAnim->mNodeName.data, nodeNameCStr, size) == 0) {
			return nodeAnim;
		}
	}
	return nullptr;
}

const unordered_map<std::string, int>& MeshData::getBoneIds() const {
	return boneIds;
}

const vector<Bone>& MeshData::getBones() const {
	return bones;
}

const int MeshData::getNumAnimations() const {
	return (int)scene->mNumAnimations;
}

const aiAnimation *MeshData::getAnimation(int index) const {
	return scene->mAnimations[index];
}

aiNode *MeshData::getSceneRoot() const {
	return scene->mRootNode;
}

void MeshData::draw(Shader & shader) const {
	if (VAO) {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

MeshData::~MeshData() {
	if (VAO) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		VAO = 0;
	}
}
