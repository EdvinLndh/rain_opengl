#version 330 core
out vec4 FragColor;
in vec4 PColor;
in vec2 TexCoords;

uniform sampler2D DropTexture;

void main()
{    

    FragColor = texture(DropTexture, TexCoords);
}