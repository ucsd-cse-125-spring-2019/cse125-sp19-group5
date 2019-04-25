#version 330 core

in vec2 texCoords;

uniform vec2 blurSize;
uniform sampler2D sourceTex;

void main() {
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * blurSize;
    vec2 off2 = vec2(3.2307692308) * blurSize;
    color += texture2D(sourceTex, texCoords) * 0.2270270270;
    color += texture2D(sourceTex, texCoords + off1) * 0.3162162162;
    color += texture2D(sourceTex, texCoords - off1) * 0.3162162162;
    color += texture2D(sourceTex, texCoords + off2) * 0.0702702703;
    color += texture2D(sourceTex, texCoords - off2) * 0.0702702703;

    gl_FragColor = color;
}