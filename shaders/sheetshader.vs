#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aInstancePos;
layout (location = 3) in int aFrameIndex;

out vec2 TexCoords;
flat out int frameIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aTexCoords;
    frameIndex = aFrameIndex;
	gl_Position = projection * view * model * vec4(aPos + aInstancePos, 1.0);
}