#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

constexpr auto IDENTITY = mat4(1.0f);

#define GET_FRAME_NUM(frame, size) \
unsigned int frame = 0, _upperFrame = nodeAnim->mNum##size##Keys; \
while (frame != _upperFrame) { \
	auto mid = (frame + _upperFrame) / 2; \
	if (keys[mid + 1].mTime <= time) { \
		frame = mid + 1; \
	} else { \
		_upperFrame = mid; \
	} \
}

Mesh::Mesh(MeshData *data) : data(data) {
	if (!data) {
		throw std::runtime_error("Mesh created with null mesh data");
	}
	for (size_t i = 0; i < data->getBones().size(); i++) {
		boneTransformations.push_back(mat4(1.0f));
	}
}

void Mesh::updateAnimation(float time) {
	if (animationId < 0 || animationId >= data->getNumAnimations()) { return; }

	auto animation = data->getAnimation(animationId);
	time = fmod(
		time * (float)animation->mTicksPerSecond,
		(float)animation->mDuration
	);
	mat4 identity(1.0f);
	buildBoneTransformations(time-1, data->getSceneRoot(), identity);
}

void interpolateTranslation(float time, aiNodeAnim *nodeAnim, mat4 &transform) {
	if (nodeAnim->mNumPositionKeys == 1) {
		auto pos = nodeAnim->mPositionKeys[0].mValue;
		transform *= glm::translate(glm::mat4(1.0f), vec3(pos.x, pos.y, pos.z));
		return;
	}

	auto keys = nodeAnim->mPositionKeys;
	GET_FRAME_NUM(frame, Position);

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
	GET_FRAME_NUM(frame, Rotation);

	auto start = keys[frame], end = keys[frame + 1];
	auto t = (time - start.mTime) / (end.mTime - start.mTime);

	aiQuaternion res;
	aiQuaternion::Interpolate(res, start.mValue, end.mValue, (float)t);

	transform *= glm::mat4_cast(glm::quat(res.w, res.x, res.y, res.z));
}

void interpolateScale(float time, aiNodeAnim *nodeAnim, mat4 &transform) {
	if (nodeAnim->mNumScalingKeys == 1) {
		auto s= nodeAnim->mScalingKeys[0].mValue;
		transform *= glm::scale(glm::mat4(1.0f), vec3(s.x, s.y, s.z));
		return;
	}

	auto keys = nodeAnim->mScalingKeys;
	GET_FRAME_NUM(frame, Scaling);

	auto start = keys[frame], end = keys[frame + 1];
	auto t = (time - start.mTime) / (end.mTime - start.mTime);
	
	auto startScale = vec3(start.mValue.x, start.mValue.y, start.mValue.z);
	auto endScale = vec3(end.mValue.x, end.mValue.y, end.mValue.z);

	transform *= glm::scale(mat4(1.0f), glm::mix(startScale, endScale, t));
}
#include <glm/gtx/string_cast.hpp>
void Mesh::buildBoneTransformations(float time, aiNode *node, const mat4 &parentTransform) {
	const auto animation = data->getAnimation(animationId);
	mat4 transformation;
	std::string nodeName(node->mName.data);

	auto nodeAnim = data->getNodeAnim(animation, nodeName);
	if (nodeAnim) {
		transformation = mat4(1.0f);
		interpolateTranslation(time, nodeAnim, transformation);
		interpolateRotation(time, nodeAnim, transformation);
		interpolateScale(time, nodeAnim, transformation);
	} else {
		transformation = toGlm(node->mTransformation);
	}

	const auto &boneIds = data->getBoneIds();
	auto boneIdIt = boneIds.find(nodeName);
	transformation = parentTransform * transformation;

	if (boneIdIt != boneIds.end()) {
		auto boneId = boneIdIt->second;
		boneTransformations[boneId] = transformation
			* data->getBones()[boneId].offset;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		buildBoneTransformations(time, node->mChildren[i], transformation);
	}
}

void Mesh::draw(Shader &shader) const {
	const auto numBones = data->getBones().size();
	for (size_t i = 0; i < numBones; i++) {
		const auto name = "boneTransforms[" + std::to_string(i) + "]";
		shader.setUniform(name.c_str(), boneTransformations[i]);
	}
	for (size_t i = numBones - 1; i < BONES_PER_VERTEX; i++) {
		const auto name = "boneTransform[" + std::to_string(i) + "]";
		shader.setUniform(name.c_str(), IDENTITY);
	}
	shader.setUniform("animated", animationId >= 0);
	data->draw(shader);
}
