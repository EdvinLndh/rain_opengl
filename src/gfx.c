#include "gfx.h"

#include "global.h"

static void use_shader(GfxSystem *g, enum ShaderType shader);
static void set_view_projection(GfxSystem *g, Shader s, bool cubemap);

void gfx_init(GfxSystem *g)
{
    g->cam = camera_create();

    g->shaders[SHADER_TEXTURE] = shader_compile("shaders/textureshader.vs", "shaders/textureshader.fs", NULL);
    g->shaders[SHADER_SHEET] = shader_compile("shaders/sheetshader.vs", "shaders/sheetshader.fs", NULL);
    g->shaders[SHADER_SKY] = shader_compile("shaders/skyshader.vs", "shaders/skyshader.fs", NULL);
    g->shaders[SHADER_PARTICLE] = shader_compile("shaders/particleshader.vs", "shaders/particleshader.fs", NULL);

    g->textures[TEXTURE_RAIN] = *load_texture("res/textures/drop.png", true, false);
    g->textures[TEXTURE_PLANE] = *load_texture("res/textures/metal.png", true, false);

    const char *cubemap_paths[] = {
        "res/textures/posx.jpg",
        "res/textures/negx.jpg",
        "res/textures/posy.jpg",
        "res/textures/negy.jpg",
        "res/textures/posz.jpg",
        "res/textures/negz.jpg"};

    g->cubeID = load_cubemap(cubemap_paths);

    g->sheets[SHEET_SPLASH] = *load_texture_sheet("res/textures/splash.png", 32, 16);

    g->vao = vao_create();
    g->vbo = vbo_create(GL_ARRAY_BUFFER, true);
    g->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);

    g->clear_color = VEC4S(0.3, 0.3, 0.3, 1.0);
}

void gfx_render_prepare(GfxSystem *g)
{
    glClearColor(g->clear_color.x, g->clear_color.y, g->clear_color.z, g->clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

static void use_shader(GfxSystem *g, enum ShaderType shader)
{
    if (g->current_shader == shader)
        return;

    g->current_shader = shader;
    shader_use(g->shaders[shader]);
}

static void set_view_projection(GfxSystem *g, Shader s, bool cubemap)
{

    mat4s view = get_view_matrix(g->cam);
    set_mat4(s, "view", cubemap ? glms_mat4_ins3(glms_mat4_pick3(view), GLMS_MAT4_IDENTITY) : view);

    set_mat4(s, "projection", glms_perspective(glm_rad(g->cam->zoom), ((float)state.window->size.x / (float)state.window->size.y), 0.1f, 100.0f));
}

void gfx_render_quad_texture(GfxSystem *g, enum TextureType tex, vec2s size,
                             vec2s uv_min, vec2s uv_max, mat4s model)
{

    use_shader(g, SHADER_TEXTURE);
    Shader s = g->shaders[g->current_shader];

    set_mat4(s, "model", model);
    set_view_projection(g, s, false);
    set_int(s, "tex", 0);
    glActiveTexture(GL_TEXTURE0);
    texture_bind(&g->textures[tex]);

    vbo_buffer(g->vbo, (float[]){0, 0, 0, 0, 0, size.y, size.x, 0, size.y, size.x, 0, 0,

                                 uv_min.x, uv_min.y, uv_min.x, uv_max.y, uv_max.x, uv_max.y, uv_max.x, uv_min.y},
               ((4 * 3) + (4 * 2)) * sizeof(float));

    vbo_buffer(g->ibo, (unsigned int[]){3, 0, 1, 3, 1, 2}, 6 * sizeof(unsigned int));

    vao_attr(g->vao, g->vbo, 0, 3, GL_FLOAT, 0, 0);
    vao_attr(g->vao, g->vbo, 1, 2, GL_FLOAT, 0, (4 * 3) * sizeof(float));
    vao_bind(g->vao);
    vbo_bind(g->ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
}

void gfx_render_cubemap(GfxSystem *g)
{
    use_shader(g, SHADER_SKY);
    Shader s = g->shaders[g->current_shader];
    set_view_projection(g, s, true);
    set_int(s, "skybox", 0);

    float vertices[] = {
        -1.0f, 1.0f, -1.0f,  // 0
        -1.0f, -1.0f, -1.0f, // 1
        1.0f, -1.0f, -1.0f,  // 2
        1.0f, 1.0f, -1.0f,   // 3
        -1.0f, 1.0f, 1.0f,   // 4
        -1.0f, -1.0f, 1.0f,  // 5
        1.0f, -1.0f, 1.0f,   // 6
        1.0f, 1.0f, 1.0f     // 7
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, // Back face
        4, 5, 1, 1, 0, 4, // Left face
        3, 2, 6, 6, 7, 3, // Right face
        5, 4, 7, 7, 6, 5, // Front face
        4, 0, 3, 3, 7, 4, // Top face
        1, 5, 6, 6, 2, 1  // Bottom face
    };

    vbo_buffer(g->vbo, vertices, sizeof(float) * (3 * 8));
    vbo_buffer(g->ibo, indices, sizeof(unsigned int) * (6 * 6));

    vao_attr(g->vao, g->vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), 0);

    glDepthFunc(GL_LEQUAL);
    vao_bind(g->vao);
    vbo_bind(g->ibo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g->cubeID);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

}

void gfx_render_particle(GfxSystem *g, ParticleSystem *p, float dt)
{

    use_shader(g, SHADER_SHEET);
    Shader s = g->shaders[g->current_shader];
    set_view_projection(g, s, false);
    set_vec3(s, "cameraRight", g->cam->right);
    set_vec3(s, "cameraFront", g->cam->front);
    update_particles(p, dt, VEC2S(g->cam->pos.x, g->cam->pos.z));



    use_shader(g, SHADER_PARTICLE);
    s = g->shaders[g->current_shader];
    set_view_projection(g, s, false);
    set_vec3(s, "cameraRight", g->cam->right);
    set_vec3(s, "cameraFront", g->cam->front);
    draw_particle(p);
}
