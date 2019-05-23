#version 330 core

in vec2 texCoords;
in vec4 vertexColor;
out vec4 fragColor;

uniform sampler2D texSampler;

void main() {
    vec4 col = texture(texSampler, texCoords);
    fragColor = col * vertexColor;
}
