#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fragPos;
out vec3 fragNormal;

uniform mat4 mvp;

void main() {
	fragPos = position;
	fragNormal = normal;

	gl_Position = mvp * vec4(fragPos, 1.0f);
}
