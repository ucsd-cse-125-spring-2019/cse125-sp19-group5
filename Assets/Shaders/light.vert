#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

uniform mat3 modelInvT;
uniform mat4 model;
uniform mat4 mvp;

void main() {
	fragPos = vec3(model * vec4(position, 1.0f));
	fragNormal = modelInvT * normal;
	fragTexCoords = texCoords;
	gl_Position = mvp * vec4(position, 1.0f);
}
