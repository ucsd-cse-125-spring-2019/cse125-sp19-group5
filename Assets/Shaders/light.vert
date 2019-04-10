#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fragPos;
out vec3 fragNormal;

uniform mat3 modelInvT;
uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(position, 1.0f);
	fragPos = vec3(gl_Position);
	fragNormal = mat3(modelInvT) * normal;
}
