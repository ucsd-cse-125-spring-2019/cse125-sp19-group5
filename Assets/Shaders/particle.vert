#version 330 core

layout(location=0) in vec3 Vertex;
layout(location=1) in vec4 PosSize;
layout(location=2) in vec4 Color;

out vec3 fragPosition;
out vec4 fragColor;

uniform mat4 modelMtx=mat4(1);
uniform mat4 modelViewProjMtx=mat4(1);

void main() {
    vec3 Position = (Vertex );
    gl_Position = modelViewProjMtx * vec4(Position,1);

    fragPosition = vec3(modelMtx * vec4(Position,1));
    fragColor = Color;
}
