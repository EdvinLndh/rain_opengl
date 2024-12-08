#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset; 

out vec4 PColor;
out vec2 TexCoords;

// uniform mat4 model;
uniform vec3 cameraRight; 
uniform vec3 cameraFront; 
uniform mat4 view;
uniform mat4 projection;

void main()
{
    PColor = aColor;
    TexCoords = aTexCoords;

    // Get the billboard direction on the XZ plane

    // Calculate the world position of the quad's vertex
    vec3 worldPosition = aOffset + (cameraRight * aPos.x) + (cameraFront * aPos.z)+ vec3(0.0, aPos.y, 0.0);

    // Convert world position to clip space
    gl_Position = projection * view * vec4(worldPosition, 1.0);

}
