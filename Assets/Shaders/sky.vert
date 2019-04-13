#version 330 core

layout (location = 0) vec3 texCoords;

out vec3 fragTexCoords;

uniform mat4 mvp;

void main() {
    fragTexCoords = texCoords;
    gl_Position = mvp * vec4(position, 1.0f);
}
