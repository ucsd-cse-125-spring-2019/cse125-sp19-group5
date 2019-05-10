#version 330 core

const int SHADOW_NUM_CASCADES = 1;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 bones;
layout (location = 4) in vec4 weights;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec3 bitangent;

out mat3 tbn;
out vec4 lightSpacePos[SHADOW_NUM_CASCADES];
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

uniform bool animated;
uniform mat3 modelInvT;
uniform mat4 model;
uniform mat4 mvp;
uniform mat4 toLightSpace[SHADOW_NUM_CASCADES];

const int MAX_BONES = 128;
uniform mat4 boneTransforms[MAX_BONES];

void main() {
	vec4 finalPos = vec4(position, 1.0f);
	vec3 finalNorm = normal;
	if (animated && weights[0] > 0.0f) {
		mat4 boneTransform = boneTransforms[bones[0]] * weights[0]
			+ boneTransforms[bones[1]] * weights[1]
			+ boneTransforms[bones[2]] * weights[2]
			+ boneTransforms[bones[3]] * weights[3];
		mat3 boneTransformInvT = transpose(inverse(mat3(boneTransform)));

		finalPos = boneTransform * finalPos;
		finalNorm = boneTransformInvT * normal;
	}
	fragPos = vec3(model * finalPos);

	vec4 fragPos4 = vec4(fragPos, 1.0f);
	for (int cascade = 0; cascade < SHADOW_NUM_CASCADES; cascade++) {
		lightSpacePos[cascade] = toLightSpace[cascade] * fragPos4;
	}

	fragNormal = modelInvT * finalNorm;
	fragTexCoords = texCoords;
	gl_Position = mvp * finalPos;

	vec3 t = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 b = normalize(vec3(model * vec4(bitangent, 0.0)));
	vec3 n = normalize(vec3(model * vec4(finalNorm, 0.0)));
	tbn = mat3(t, b, n);
}
