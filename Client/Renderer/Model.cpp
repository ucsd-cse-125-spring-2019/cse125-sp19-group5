#include "Model.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Model::Model(const std::string &path) {
	// Create a scene from the given path to the model file.
	Assimp::Importer importer;
	auto scene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs
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

void Model::draw(const Shader &shader) const {
	for (auto mesh : meshes) {
		mesh.draw(shader);
	}
}

void Model::loadNode(aiNode *node, const aiScene *scene) {
	// Convert the current node to meshes.
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(loadMesh(mesh, scene));
	}

	// Then, recursively load the node's children.
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		loadNode(node->mChildren[i], scene);
	}
}

std::vector<Vertex> Model::loadMeshVertices(aiMesh *mesh) {
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

std::vector<ElementIndex> Model::loadMeshIndices(aiMesh *mesh) {
	std::vector<ElementIndex> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	return indices;
}

static mat4 toGlm(const aiMatrix4x4 &m) {
	return glm::transpose(glm::make_mat4(&m.a1));
}

void Model::loadMeshBones(aiMesh *mesh, std::vector<Vertex> &vertices) {
	std::unordered_map<std::string, int> boneIds;
	std::vector<Bone> bones;

	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		auto boneInfo = mesh->mBones[i];
		auto boneIndex = 0;

		// Get the unique ID for the bone by name.
		std::string boneName(boneInfo->mName.data);
		auto boneIdIt = boneIds.find(boneName);
		if (boneIdIt == boneIds.end()) {
			boneIndex = (int)bones.size();

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

Mesh Model::loadMesh(aiMesh *mesh, const aiScene *scene) {
	auto vertices = loadMeshVertices(mesh);
	auto indices = loadMeshIndices(mesh);
	loadMeshBones(mesh, vertices);
	return Mesh(vertices, indices);
}
