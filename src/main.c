#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "audio.h"
#include "global.h"
#include "gfx.h"
#include "particle.h"
#include "util.h"

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void process_input(GLFWwindow *window, float delta);
void update_rain();
void create_particles(ParticleConfig *cfg);

void toggle_mouse_capture(GLFWwindow *window);


ParticleSystem **particleSystems = NULL; 
unsigned int particleSystemCount = 0;    
unsigned int particleSystemCapacity = 0;

// Globals
Global state; 

int main()
{
    state.window = &window;
    window_init();
    audio_init();
    Audio_t *rain = audio_open("res/audio/light_rain.ogg");
    audio_start(rain);
    // imgui_init(state.window->handle, "#version 130");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    state.gfx = malloc(sizeof(GfxSystem));
    gfx_init(state.gfx);

    create_particles(&(ParticleConfig){
        .num_particles = 1000,
        .color = VEC4S(0.3f, 0.3f, 0.6f, 1.0f),
        .type = RAIN,
        .cam_rad = VEC3S(30.0f, 20.0f, 30.0f),
    });

    while (!glfwWindowShouldClose(state.window->handle))
    {

        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // manage user input
        // -----------------
        process_input(state.window->handle, deltaTime);

        gfx_render_prepare(state.gfx);

        // imgui_start_frame();

        // render
        // ------

        mat4s model = glms_mat4_identity();
        gfx_render_quad_texture(state.gfx, TEXTURE_PLANE, VEC2S(10.0, 10.0), VEC2S(0.0, 0.0), VEC2S(1.0, 1.0), model);

        // gfx_render_cubemap(state.gfx);
        for (unsigned int i = 0; i < particleSystemCount; ++i)
        {
            gfx_render_particle(state.gfx, particleSystems[i], deltaTime);
        }

        // imgui_render(window);

        glfwSwapBuffers(state.window->handle);
        glfwPollEvents();
    }

    glfwTerminate();
    // imgui_cleanup();
    audio_kill(rain);
    return 0;
}


void create_particles(ParticleConfig *cnf)
{
    if (particleSystemCount >= particleSystemCapacity)
    {
        particleSystemCapacity = (particleSystemCapacity == 0) ? 1 : particleSystemCapacity * 2;

        particleSystems = realloc(particleSystems, particleSystemCapacity * sizeof(ParticleSystem *));
        if (!particleSystems)
        {
            fprintf(stderr, "Failed to allocate memory for ParticleSystem array\n");
            exit(EXIT_FAILURE);
        }
    }

    ParticleSystem *p = particles_create(cnf);

    particleSystems[particleSystemCount++] = p;
}
