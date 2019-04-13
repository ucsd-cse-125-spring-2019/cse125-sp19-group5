#version 330 core

in vec3 fragTexCoords;
out vec4 fragColor;

uniform sampleCube cubemap;

void main() {
    fragColor = texture(cubemap, fragTexCoords);
}
