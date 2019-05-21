#version 330 core

in vec3 fragPosition;
in vec4 fragColor;

uniform bool RenderingPlane;
uniform vec3 AmbientColor;

out vec3 finalColor;

void main() {
    // if (RenderingPlane)
    //     gl_FragColor = vec4(AmbientColor, 1.0f);
    // else
        gl_FragColor = vec4(1.0);
}
