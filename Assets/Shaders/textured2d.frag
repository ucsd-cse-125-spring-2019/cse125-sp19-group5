#version 330 core

in vec2 TexCoords;

out vec4 fragColor;

uniform vec4 color;
uniform sampler2D text;

void main()
{    
    vec2 fixedTexCords = vec2(TexCoords.x, 1.0 - TexCoords.y);
    fragColor = color * texture(text, fixedTexCords);
}  
