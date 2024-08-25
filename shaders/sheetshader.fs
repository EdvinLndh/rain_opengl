#version 330 core

in vec2 TexCoords;
flat in int frameIndex;

uniform sampler2D sheet;

out vec4 FragColor;

const int NUM_SPRITES = 4;

void main(void) {

    // Calculate sprite width in texture coordinates
    float spriteWidth = 1.0 / float(NUM_SPRITES);

    // Calculate the starting X coordinate for the current sprite
    float xOffset = spriteWidth * float(frameIndex);

    // Adjust the texture coordinates to sample the correct sprite
    vec2 spriteCoords = vec2(xOffset + TexCoords.x * spriteWidth, TexCoords.y);
    FragColor = texture(sheet, spriteCoords);
}