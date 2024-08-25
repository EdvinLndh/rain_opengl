#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "imgui.h"

#include "camera.h"
#include "vao.h"
#include "vbo.h"
#include "shader.h"
#include "texture.h"
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
void create_particles();

void toggle_mouse_capture(GLFWwindow *window);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;

bool firstMouse = true;

bool mouse_captured = true;

bool keys[1024];

// Camera *c = NULL;

ParticleSystem **particleSystems = NULL; // Dynamic array of ParticleSystem pointers
unsigned int particleSystemCount = 0;    // Number of currently active ParticleSystems
unsigned int particleSystemCapacity = 0;

GfxSystem *gfx; 

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rain", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    imgui_init(window, "#version 130");


    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    gfx = malloc(sizeof(GfxSystem));
    gfx_init(gfx);
    

    // ImVec4 clearColor = {0.2f, 0.3f, 0.4f, 1.00f};
    // int num_particles = 1000;

    // float starx = 100.0f, stary = 200.0f, starz = 100.0f;
    // float rainx = 30.0f, rainy = 40.0f, rainz = 30.0f;

    // vec4s color = VEC4S(0.72f, 0.78f, 0.0f, 1.0f);
    // int i = 1;
        create_particles(&(ParticleConfig) {
            .num_particles = 1000,
            .color = VEC4S(0.3f, 0.3f, 0.6f, 1.0f),
            .type = RAIN,
            .x = 20.0f, 
            .y = 50.0f, 
            .z = 20.0f,
        });

    while (!glfwWindowShouldClose(window))
    {

        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // manage user input
        // -----------------
        process_input(window, deltaTime);

        gfx_render_prepare(gfx);

        imgui_start_frame();

        // Your ImGui code goes here
        // igColorEdit3("clear color", (float *)&clearColor, 0);

        // imgui_treenode_particle("Create stars", num_particles, rainx, rainy, rainz, color, pshader, *create_particles);
        // imgui_treenode_particle("Create rain", num_particles, rainx, rainy, rainz, color, pshader, *create_particles);



        // Render ImGui

        // render
        // ------

        mat4s model = glms_mat4_identity();
        gfx_render_quad_texture(gfx, TEXTURE_PLANE, VEC2S(10.0, 10.0), VEC2S(0.0, 0.0), VEC2S(1.0, 1.0), model);

        gfx_render_cubemap(gfx);
        for (unsigned int i = 0; i < particleSystemCount; ++i)
        {
            gfx_render_particle(gfx, particleSystems[i], deltaTime);
        }

        imgui_render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    glfwTerminate();
    imgui_cleanup();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    process_scroll(gfx->cam, yoffset);
}

void process_input(GLFWwindow *window, float delta)
{
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);
    if (keys[GLFW_KEY_W])
        process_keyboard(gfx->cam, FORWARD, delta);
    if (keys[GLFW_KEY_S])
        process_keyboard(gfx->cam, BACKWARD, delta);
    if (keys[GLFW_KEY_A])
        process_keyboard(gfx->cam, LEFT, delta);
    if (keys[GLFW_KEY_D])
        process_keyboard(gfx->cam, RIGHT, delta);
    if (keys[GLFW_KEY_LEFT_SHIFT])
        process_keyboard(gfx->cam, SHIFT, delta);
    if (keys[GLFW_KEY_TAB])
        toggle_mouse_capture(window);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    process_mouse(gfx->cam, xoffset, yoffset, true);
}

void toggle_mouse_capture(GLFWwindow *window)
{
    mouse_captured = !mouse_captured;
    if (mouse_captured)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetCursorPosCallback(window, mouse_callback);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, NULL);
    }
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
