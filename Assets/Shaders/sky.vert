#version 330 core

layout (location = 0) in vec3 texCoords;

out vec3 fragTexCoords;

uniform mat4 mvp;

void main() {
    fragTexCoords = texCoords;
    gl_Position = (mvp * vec4(texCoords, 1.0f)).xyww;
}
