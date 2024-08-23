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

    // c = camera_create();
    gfx = malloc(sizeof(GfxSystem));
    gfx_init(gfx);
    
    // float planeVertices[] = {
    //     // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    //     5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
    //     -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
    //     -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

    //     5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
    //     -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
    //     5.0f, -0.5f, -5.0f, 2.0f, 2.0f};

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    VAO cubeVAO = vao_create();
    VBO cubeVBO = vbo_create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    vbo_buffer(cubeVBO, skyboxVertices, sizeof(skyboxVertices));

    vao_attr(cubeVAO, cubeVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), 0);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    char *texture_faces[] = {
        "res/textures/posx.jpg",
        "res/textures/negx.jpg",
        "res/textures/posy.jpg",
        "res/textures/negy.jpg",
        "res/textures/posz.jpg",
        "res/textures/negz.jpg"};
    int width, height, nrChannels;
    unsigned char *data;
    for (unsigned int i = 0; i < (sizeof(texture_faces) / sizeof(char *)); i++)
    {
        data = stbi_load(texture_faces[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            fprintf(stderr, "Cubemap failed to load\n");
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // shader_use(skyboxShader);
    // set_int(skyboxShader, "skybox", 0);

    ImVec4 clearColor = {0.2f, 0.3f, 0.4f, 1.00f};
    int num_particles = 1000;

    float starx = 100.0f, stary = 200.0f, starz = 100.0f;
    float rainx = 30.0f, rainy = 40.0f, rainz = 30.0f;

    vec4s color = VEC4S(0.72f, 0.78f, 0.0f, 1.0f);
    int i = 1;

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
        gfx_render_quad_texture(gfx, TEXTURE_PLANE, VEC2S(10.0, 10.0), VEC2S(1.0, 0.0), VEC2S(0.0, 1.0), model);

        // shader_use(texture_shader);
        // set_mat4(texture_shader, "view", view);
        // set_mat4(texture_shader, "projection", projection);
        // set_mat4(texture_shader, "model", model);

        

        // vao_bind(planeVAO);
        // glActiveTexture(GL_TEXTURE0);
        // texture_bind(tex);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);

        // glDepthFunc(GL_LEQUAL);
        // shader_use(skyboxShader);

        // ... set view and projection matrix
        // set_mat4(skyboxShader, "view", glms_mat4_ins3(glms_mat4_pick3(view), GLMS_MAT4_IDENTITY));
        // set_mat4(skyboxShader, "projection", projection);
        // vao_bind(cubeVAO);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);
        // glDepthFunc(GL_LESS);

        // for (unsigned int i = 0; i < particleSystemCount; ++i)
        // {
        //     Shader sheet_s = particleSystems[i]->sheet_system.shader;
        //     shader_use(sheet_s);

        //     set_mat4(sheet_s, "view", view);
        //     set_mat4(sheet_s, "projection", projection);
        //     set_mat4(sheet_s, "model", model);
        //     update_particles(particleSystems[i], deltaTime);
        //     shader_use(pshader);
        //     set_mat4(pshader, "view", view);
        //     set_mat4(pshader, "projection", projection);
        //     set_vec3(pshader, "cameraRight", c->right);
        //     set_vec3(pshader, "cameraFront", c->front);
        //     draw_particle(particleSystems[i], pshader);
        // }

        imgui_render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    // Map
    // GL objects (VBA, VAO..)

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
