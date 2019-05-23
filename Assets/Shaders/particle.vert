#version 330 core

layout(location=0) in vec3 Vertex;
layout(location=1) in vec4 PosSize;
layout(location=2) in vec4 Color;

out vec2 texCoords;
out vec4 vertexColor;

uniform vec3 camUp;
uniform vec3 camRight;

uniform mat4 modelMtx=mat4(1);
uniform mat4 modelViewProjMtx=mat4(1);

void main() {
    gl_Position = modelViewProjMtx * vec4(
        PosSize.xyz
        + camRight * Vertex.x * PosSize.w
        + camUp * Vertex.y * PosSize.w,
        1.0
    );

    vertexColor = Color;
    texCoords = Vertex.xy + vec2(0.5);
}
