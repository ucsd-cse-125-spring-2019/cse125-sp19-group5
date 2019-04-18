#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 bones;
layout (location = 4) in vec4 weights;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

uniform mat3 modelInvT;
uniform mat4 model;
uniform mat4 mvp;

const int MAX_BONES = 128;
uniform mat4 boneTransforms[MAX_BONES];

void main() {
	mat4 boneTransform = boneTransforms[bones[0]] * weights[0]
		+ boneTransforms[bones[1]] * weights[1]
		+ boneTransforms[bones[2]] * weights[2]
		+ boneTransforms[bones[3]] * weights[3];
	vec3 finalPos = vec3(boneTransform * vec4(position, 1.0f));
	vec3 finalNorm = vec3(boneTransform * vec4(normal, 0.0f));
	fragPos = vec3(model * vec4(finalPos, 1.0f));
	fragNormal = modelInvT * finalNorm;
	fragTexCoords = texCoords;
	gl_Position = mvp * vec4(finalPos, 1.0f);
}
