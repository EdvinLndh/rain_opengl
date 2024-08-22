#version 330 core

in vec2 TexCoords;
flat in int frameIndex;

uniform sampler2D sheet;

out vec4 FragColor;

const int NUM_SPRITES = 4;

void main(void) {
    // vec2 pos = vec2(sprite_idx % int(SPRITE_COLUMNS), int(sprite_idx / SPRITE_COLUMNS));
    // FragColor = texture(sheet, vec2((TexCoords.x / SPRITE_COLUMNS) + pos.x * (1.0 / SPRITE_COLUMNS),
    //     (TexCoords.y / SPRITE_ROWS) + pos.y * (1.0 / SPRITE_ROWS)));

    // Calculate sprite width in texture coordinates
    float spriteWidth = 1.0 / float(NUM_SPRITES);

    // Calculate the starting X coordinate for the current sprite
    float xOffset = spriteWidth * float(frameIndex);

    // Adjust the texture coordinates to sample the correct sprite
    vec2 spriteCoords = vec2(xOffset + TexCoords.x * spriteWidth, TexCoords.y);
    FragColor = texture(sheet, spriteCoords);
}