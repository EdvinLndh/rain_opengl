#ifndef GFX_H
#define GFX_H

#include <stdbool.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <cglm/types-struct.h>
#include <string.h>

#include "Heightmap.h"
#include "vao.h"
#include "vbo.h"
#include "particle.h"
#include "shader.h"
#include "util.h"
#include "texture.h"
#include "camera.h"


#define SHADERS_LAST SHADER_PARTICLE
enum ShaderType {
    SHADER_NONE = 0,
    SHADER_TEXTURE, 
    SHADER_SHEET,
    SHADER_SKY,
    SHADER_PARTICLE,
};

#define TEXTURE_LAST TEXTURE_RAIN
enum TextureType {
    TEXTURE_PLANE = 0, 
    TEXTURE_RAIN,
};

#define SHEET_LAST SHEET_SPLASH
enum SheetType {
    SHEET_SPLASH = 0,
};



typedef struct GfxSytem {
    
    Camera *cam; 

    Shader shaders[SHADERS_LAST + 1];
    enum ShaderType current_shader; 
    Texture2D textures[TEXTURE_LAST + 1];
    TextureSheet sheets[SHEET_SPLASH + 1];
    unsigned int cubeID;

    VAO vao;
    VBO vbo, ibo; 

    vec4s clear_color;

    struct {
        bool wireframe : 1; 
    } flags;


} GfxSystem;




void gfx_init(GfxSystem *g);

void gfx_render_prepare(GfxSystem *g);
void gfx_render_quad_texture(GfxSystem *g, enum TextureType tex, vec2s size, 
        vec2s uv_min, vec2s uv_max, mat4s model);

void gfx_render_cubemap(GfxSystem *g);
void gfx_render_heightmap(GfxSystem *g, HeightMap *hm);
// void gfx_render_fog(GfxSystem *g);
void gfx_render_particle(GfxSystem *g, ParticleSystem *p, float dt);
#endif // !GFX_H
