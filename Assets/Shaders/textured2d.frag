#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D text;

void main()
{    
    color = vec4(vec3(texture(text, TexCoords).rgb), 1.0f);
}  