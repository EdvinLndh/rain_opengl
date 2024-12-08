#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aInstancePos;
layout (location = 3) in int aFrameIndex;

out vec2 TexCoords;
flat out int frameIndex;

uniform vec3 cameraRight; 
uniform vec3 cameraFront; 
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aTexCoords;
    frameIndex = aFrameIndex;
    vec3 worldPosition = aInstancePos + (cameraRight * aPos.x) + (cameraFront * aPos.z)+ vec3(0.0, aPos.y, 0.0);

    // Convert world position to clip space
    gl_Position = projection * view * vec4(worldPosition, 1.0);
}
