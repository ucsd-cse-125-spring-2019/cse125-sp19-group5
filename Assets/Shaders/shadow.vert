#version 330 core

layout (location = 0) in vec3 position;
layout (location = 3) in ivec4 bones;
layout (location = 4) in vec4 weights;

uniform mat4 toLightSpace;
uniform mat4 model;
uniform bool animated;

const int MAX_BONES = 128;
uniform mat4 boneTransforms[MAX_BONES];

void main() {
	vec4 finalPos = vec4(position, 1.0f);
	if (animated && weights[0] > 0.0f) {
		mat4 boneTransform = boneTransforms[bones[0]] * weights[0]
			+ boneTransforms[bones[1]] * weights[1]
			+ boneTransforms[bones[2]] * weights[2]
			+ boneTransforms[bones[3]] * weights[3];
		mat3 boneTransformInvT = transpose(inverse(mat3(boneTransform)));
		finalPos = boneTransform * finalPos;
	}
    gl_Position = toLightSpace * model * finalPos;
}