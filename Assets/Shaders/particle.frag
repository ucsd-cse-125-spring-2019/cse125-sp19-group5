#version 330 core

in vec3 fragPosition;
in vec4 color;

uniform bool RenderingPlane;
uniform vec3 AmbientColor;

out vec4 fragColor;

void main() {
    // if (RenderingPlane)
    //     gl_FragColor = vec4(AmbientColor, 1.0f);
    // else
    fragColor = color;
}
