#ifndef PARTICLE_H
#define PARTICLE_H

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <cglm/types-struct.h>

#include "texture.h"
#include "shader.h"
#include "util.h"
#include "vao.h"
#include "vbo.h"

enum ParticleType
{
    DOTS,
    RAIN,
};

typedef struct SplashInstance {
    vec3s position;
    int frame_index;
    bool active;
    float elapsed_time;
} SplashInstance;

// Some redundancy here? Maybe figure out a better way to handle all this. 
typedef struct Particle
{
    vec3s pos;
    vec3s velocity;
    vec4s color;
    float size, angle, weight;
    float life;
} Particle;

typedef struct ParticleConfig
{
    unsigned int num_particles;
    float x, y, z;
    vec4s color;
    enum ParticleType type;
    Shader shader;
} ParticleConfig;

typedef struct TextureSheetSystem {
    TextureSheet *sheet; 
    Shader shader; 
    VAO vao; 
    VBO vbo; 
    VBO instanceVBO;
    SplashInstance *instances;
} TextureSheetSystem;

typedef struct ParticleSystem
{
    Particle *particles;
    ParticleConfig *conf;
    Texture2D *tex;
    TextureSheetSystem sheet_system;
    
    VAO vao;
    VBO vbo;
    VBO instanceVBO;
    vec3s *offsets; 
} ParticleSystem;

ParticleSystem *particles_create(ParticleConfig *conf);

void update_particles(ParticleSystem *p, float dt);

void draw_particle(ParticleSystem *p, Shader shader);

#endif // !PARTICLE_H
