#version 330 core

in vec2 TexCoords;

out vec4 fragColor;

uniform vec4 color;
uniform sampler2D text;

void main()
{    
    fragColor = color * vec4(vec3(texture(text, TexCoords).r), 1.0f);
}  