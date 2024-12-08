#include "particle.h"

static void update_splash_instances(ParticleSystem *p, float dt);
static void update_rain(ParticleSystem *p, float dt, vec2s cam_pos);
static void draw_splash(ParticleSystem *p);
static void gen_positions(ParticleSystem *p, int index, vec2s cam_offset);

ParticleSystem *particles_create(ParticleConfig *conf)
{
    ParticleSystem *p = malloc(sizeof(ParticleSystem));
    if (!p)
    {
        fprintf(stderr, "Failed to allocate memory for Particles\n");
        return NULL;
    }

    p->conf = conf;

    p->particles = calloc(sizeof(Particle), conf->num_particles);
    if (!p->particles)
    {
        fprintf(stderr, "Failed to allocate memory for Particle array\n");
        free(p);
        return NULL;
    }

    p->offsets = calloc(conf->num_particles, sizeof(vec3s));
    gen_positions(p, -1, VEC2S(0, 0));

    // Instanced array
    VBO instanceVBO = vbo_create(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    vbo_buffer(instanceVBO, p->offsets, sizeof(vec3s) * conf->num_particles);
    p->instanceVBO = instanceVBO;

    float vertexData[] = {
        // positions        // colors              // texture coords
        -0.08f, -0.4f, 0.0f, conf->color.x, conf->color.y, conf->color.z, conf->color.w, 1.0f, 1.0f, // Bottom-left
        0.08f, -0.4f, 0.0f, conf->color.x, conf->color.y, conf->color.z, conf->color.w, 0.0f, 1.0f,  // Bottom-right
        0.08f, 0.4f, 0.0f, conf->color.x, conf->color.y, conf->color.z, conf->color.w, 0.0f, 0.0f,   // Top-right

        -0.08f, -0.4f, 0.0f, conf->color.x, conf->color.y, conf->color.z, conf->color.w, 1.0f, 1.0f, // Bottom-left
        0.08f, 0.4f, 0.0f, conf->color.x, conf->color.y, conf->color.z, conf->color.w, 0.0f, 0.0f,   // Top-right
        -0.08f, 0.4f, 0.0f, conf->color.x, conf->color.y, conf->color.z, conf->color.w, 1.0f, 0.0f   // Top-left
    };

    p->tex = load_texture("res/textures/drop.png", true, false);
    p->vao = vao_create();
    p->vbo = vbo_create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    vbo_buffer(p->vbo, vertexData, sizeof(vertexData));

    // Set up the position attribute
    vao_attr(p->vao, p->vbo, 0, 3, GL_FLOAT, sizeof(float) * 9, 0);
    // Set up the color attribute
    vao_attr(p->vao, p->vbo, 1, 4, GL_FLOAT, sizeof(float) * 9, sizeof(float) * 3);
    vao_attr(p->vao, p->vbo, 2, 2, GL_FLOAT, sizeof(float) * 9, sizeof(float) * 7);
    vao_attr(p->vao, p->instanceVBO, 3, 3, GL_FLOAT, sizeof(vec3s), 0);
    vao_bind(p->vao);
    glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

    p->sheet_system = (TextureSheetSystem){
        .sheet = load_texture_sheet("res/textures/splash.png", 32, 16),
        .vao = vao_create(),
        .vbo = vbo_create(GL_ARRAY_BUFFER, GL_STATIC_DRAW),
        .instanceVBO = vbo_create(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW),
        .shader = shader_compile("shaders/sheetshader.vert", "shaders/sheetshader.frag", NULL),
    };

    float sheetVertexData[] = {
        // positions        // texture coords
        -0.1f, -0.1f, 0.0f, 1.0f, 1.0f, // Bottom-left
        0.1f, -0.1f, 0.0f, 0.0f, 1.0f,  // Bottom-right
        0.1f, 0.1f, 0.0f, 0.0f, 0.0f,   // Top-right

        -0.1f, -0.1f, 0.0f, 1.0f, 1.0f, // Bottom-left
        0.1f, 0.1f, 0.0f, 0.0f, 0.0f,   // Top-right
        -0.1f, 0.1f, 0.0f, 1.0f, 0.0f   // Top-left
    };

    p->sheet_system.instances = calloc(p->conf->num_particles, sizeof(SplashInstance));

    vbo_buffer(p->sheet_system.instanceVBO, p->sheet_system.instances, sizeof(SplashInstance) * conf->num_particles);
    vbo_buffer(p->sheet_system.vbo, sheetVertexData, sizeof(sheetVertexData));
    vao_attr(p->sheet_system.vao, p->sheet_system.vbo, 0, 3, GL_FLOAT, sizeof(float) * 5, 0);
    vao_attr(p->sheet_system.vao, p->sheet_system.vbo, 1, 2, GL_FLOAT, sizeof(float) * 5, sizeof(float) * 3);
    vao_attr(p->sheet_system.vao, p->sheet_system.instanceVBO, 2, 3, GL_FLOAT, sizeof(SplashInstance), offsetof(SplashInstance, position));
    vao_attr(p->sheet_system.vao, p->sheet_system.instanceVBO, 3, 1, GL_INT, sizeof(SplashInstance), offsetof(SplashInstance, frame_index));
    vao_bind(p->sheet_system.vao);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    shader_use(p->sheet_system.shader);
    set_int(p->sheet_system.shader, "sheet", 0);

    return p;
}

static void gen_positions(ParticleSystem *p, int index, vec2s cam_offset)
{

    if (index >= 0) {
        vec3s *offset = &p->offsets[index]; 
        float x = (((float)rand() / RAND_MAX) * p->conf->cam_rad.x * 2) - p->conf->cam_rad.x;
        float z = (((float)rand() / RAND_MAX) * p->conf->cam_rad.z * 2) - p->conf->cam_rad.z;
        offset->x = (x + cam_offset.x); 
        offset->z = z + cam_offset.y;
        return;
    }

    for (unsigned int i = 0; i < p->conf->num_particles; ++i)
    {
        float x = (((float)rand() / RAND_MAX) * p->conf->cam_rad.x * 2) - p->conf->cam_rad.x;
        float y = (((float)rand() / RAND_MAX) * p->conf->cam_rad.y * 2) + 5.0f;
        float z = (((float)rand() / RAND_MAX) * p->conf->cam_rad.z * 2) - p->conf->cam_rad.z;

        p->offsets[i] = VEC3S(x, y, z);

        Particle particle = (Particle){
            .pos = VEC3S(p->conf->cam_rad.x, p->conf->cam_rad.y, p->conf->cam_rad.z),
            .life = 0.0f,
            .velocity = VEC3S(0.0f, -(((float)rand() / RAND_MAX) * 5 + 13), 0.0f),
            .color = p->conf->color};
        p->particles[i] = particle;
    }
}

void update_particles(ParticleSystem *p, float dt, vec2s cam_pos)
{
    switch (p->conf->type)
    {
    case RAIN:
        update_rain(p, dt, cam_pos);
        break;
    default:
        break;
    }
}

void draw_particle(ParticleSystem *p)
{
    vao_bind(p->vao);
    glActiveTexture(GL_TEXTURE0);
    texture_bind(p->tex);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, p->conf->num_particles);
    glBindVertexArray(0);
}

static float elapsed_time = 0.0f;
static void update_rain(ParticleSystem *p, float dt, vec2s cam_pos)
{
    elapsed_time += dt;
    for (unsigned int i = 0; i < p->conf->num_particles; i++)
    {
        vec3s *offset = &p->offsets[i];
        offset->y += (p->particles[i].velocity.y * dt);
        if (offset->y <= -1.0f)
        {
            // Play splash effect
            p->sheet_system.instances[i] = (SplashInstance){
                .frame_index = 1,
                .position = *offset,
                .active = true,
                .elapsed_time = 0,
            };
            update_splash_instances(p, dt);
            draw_splash(p);

            // Reset drop position
            offset->y = p->conf->cam_rad.y;

            gen_positions(p, i, VEC2S(cam_pos.x, cam_pos.y));
        }
    }
    vbo_bind(p->instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3s) * p->conf->num_particles, p->offsets);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void draw_splash(ParticleSystem *p)
{
    vao_bind(p->sheet_system.vao);
    glActiveTexture(GL_TEXTURE0);
    texture_bind(p->sheet_system.sheet->tex);

    vbo_bind(p->sheet_system.instanceVBO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, p->conf->num_particles);
    glBindVertexArray(0);
}

static void update_splash_instances(ParticleSystem *p, float dt)
{
    float frame_duration = 0.2f;
    // fix more efficient loop in future.
    for (unsigned int i = 0; i < p->conf->num_particles; ++i)
    {
        SplashInstance *instance = &p->sheet_system.instances[i];
        if (instance->active)
        {
            instance->elapsed_time += dt;
            if (instance->elapsed_time >= frame_duration)
            {
                // Move to the next frame
                instance->elapsed_time = 0.0f;
                if (++instance->frame_index > 4)
                {

                    instance = NULL;
                }
            }
            else
                continue;
        }
    }
    vbo_bind(p->sheet_system.instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SplashInstance) * p->conf->num_particles, p->sheet_system.instances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
