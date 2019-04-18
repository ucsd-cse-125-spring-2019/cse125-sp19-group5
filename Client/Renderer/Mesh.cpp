#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

Mesh::Mesh(
	std::vector<Vertex>& vertices,
	const std::vector<ElementIndex> indices,
	aiMesh *mesh,
	const aiScene *scene
): mesh(mesh), scene(scene) {
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
	glVertexAttribIPointer(4, BONES_PER_VERTEX, GL_FLOAT, sizeof(Vertex), offset);
	glEnableVertexAttribArray(4);

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static mat4 toGlm(const aiMatrix4x4 &m) {
	return glm::transpose(glm::make_mat4(&m.a1));
}

void Mesh::loadBones(std::vector<Vertex> &vertices) {
	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		auto boneInfo = mesh->mBones[i];
		auto boneIndex = 0;

		// Get the unique ID for the bone by name.
		std::string boneName(boneInfo->mName.data);
		auto boneIdIt = boneIds.find(boneName);
		if (boneIdIt == boneIds.end()) {
			boneIds[boneName] = (int)bones.size();

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
	for (size_t i = 0; i < bones.size(); i++) {
		boneTransformations.push_back(mat4(1.0f));
	}
}

aiNodeAnim *Mesh::getNodeAnim(const aiAnimation *animation, const std::string &nodeName) {
	for (unsigned int i = 0; i < animation->mNumChannels; i++) {
		auto nodeAnim = animation->mChannels[i];
		if (std::string(nodeAnim->mNodeName.data) == nodeName) {
			return nodeAnim;
		}
	}
	return nullptr;
}

void Mesh::updateAnimation(float time) {
	auto animation = scene->mAnimations[0];
	time = fmod(
		time * (float)animation->mTicksPerSecond,
		(float)animation->mDuration
	);

	mat4 identity(1.0f);
	buildBoneTransformations(time, scene->mRootNode, identity);
}

void interpolateTranslation(float time, aiNodeAnim *nodeAnim, mat4 &transform) {
	if (nodeAnim->mNumPositionKeys == 1) {
		auto pos = nodeAnim->mPositionKeys[0].mValue;
		transform *= glm::translate(glm::mat4(1.0f), vec3(pos.x, pos.y, pos.z));
		return;
	}

	auto keys = nodeAnim->mPositionKeys;
	unsigned int frame;
	for (frame = 0; frame < nodeAnim->mNumPositionKeys - 1; frame++) {
		if (time < keys[frame + 1].mTime) {
			break;
		}
	}

	auto start = keys[frame], end = keys[frame + 1];
	auto t = (time - start.mTime) / (end.mTime - start.mTime);

	auto startPos = start.mValue;
	auto endPos = end.mValue;

	auto p0 = vec3(start.mValue.x, start.mValue.y, start.mValue.z);
	auto p1 = vec3(end.mValue.x, end.mValue.y, end.mValue.z);
	
	transform *= glm::translate(mat4(1.0f), glm::mix(p0, p1, t));
}

void interpolateRotation(float time, aiNodeAnim *nodeAnim, mat4 &transform) {
	if (nodeAnim->mNumRotationKeys == 1) {
		auto rot = nodeAnim->mRotationKeys[0].mValue;
		transform *= glm::mat4_cast(glm::quat(rot.w, rot.x, rot.y, rot.z));
		return;
	}

	auto keys = nodeAnim->mRotationKeys;
	unsigned int frame;
	for (frame = 0; frame < nodeAnim->mNumRotationKeys - 1; frame++) {
		if (time < keys[frame + 1].mTime) {
			break;
		}
	}

	auto start = keys[frame], end = keys[frame + 1];
	auto t = (time - start.mTime) / (end.mTime - start.mTime);

	auto startVal = start.mValue, endVal = end.mValue;
	glm::quat startQ(startVal.w, startVal.x, startVal.y, startVal.z);
	glm::quat endQ(endVal.w, endVal.x, endVal.y, endVal.z);
	
	transform *= glm::mat4_cast(glm::slerp(startQ, endQ, (float)t));
}

void interpolateScale(float time, aiNodeAnim *nodeAnim, mat4 &transform) {
	if (nodeAnim->mNumScalingKeys == 1) {
		auto s= nodeAnim->mScalingKeys[0].mValue;
		transform *= glm::scale(glm::mat4(1.0f), vec3(s.x, s.y, s.z));
		return;
	}

	auto keys = nodeAnim->mScalingKeys;
	unsigned int frame;
	for (frame = 0; frame < nodeAnim->mNumScalingKeys - 1; frame++) {
		if (time < keys[frame + 1].mTime) {
			break;
		}
	}

	auto start = keys[frame], end = keys[frame + 1];
	auto t = (time - start.mTime) / (end.mTime - start.mTime);
	
	auto startScale = vec3(start.mValue.x, start.mValue.y, start.mValue.z);
	auto endScale = vec3(end.mValue.x, end.mValue.y, end.mValue.z);


	transform *= glm::scale(mat4(1.0f), glm::mix(startScale, endScale, t));
}

void Mesh::buildBoneTransformations(float time, aiNode *node, const mat4 &parentTransform) {
	auto animation = scene->mAnimations[0];
	auto transformation = toGlm(node->mTransformation);
	std::string nodeName(node->mName.data);

	auto nodeAnim = getNodeAnim(animation, nodeName);
	if (nodeAnim) {
		transformation = mat4(1.0f);

		interpolateTranslation(time, nodeAnim, transformation);
		interpolateRotation(time, nodeAnim, transformation);
		interpolateScale(time, nodeAnim, transformation);
	}

	auto boneIdIt = boneIds.find(nodeName);
	transformation = parentTransform * transformation;

	if (boneIdIt != boneIds.end()) {
		auto boneId = boneIdIt->second;
		boneTransformations[boneId] = transformation
			* bones[boneId].offset;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		buildBoneTransformations(time, node->mChildren[i], transformation);
	}
}

void Mesh::draw(Shader &shader) const {
	for (size_t i = 0; i < bones.size(); i++) {
		const auto name = "boneTransforms[" + std::to_string(i) + "]";
		shader.setUniform(name.c_str(), boneTransformations[i]);
	}
	if (VAO) {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

Mesh::~Mesh() {
	if (VAO) {
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		VAO = 0;
	}
}
