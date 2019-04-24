#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 texCoords;

void main() {
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
    texCoords = vec2(vertex.z, 1.0 - vertex.w);
}