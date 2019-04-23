#version 330 core

in vec4 fragPos;

void main() {
    gl_FragColor = vec4(gl_FragCoord.z, 0.5f, 0.0f, 0.0f);
}