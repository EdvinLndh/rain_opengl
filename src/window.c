#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include "global.h"

// global window
Window window;

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;

bool firstMouse = true;

bool mouse_captured = true;

bool keys[1024];
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
    process_scroll(state.gfx->cam, yoffset);
}

void process_input(GLFWwindow *window, float delta)
{
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);
    if (keys[GLFW_KEY_W])
        process_keyboard(state.gfx->cam, FORWARD, delta);
    if (keys[GLFW_KEY_S])
        process_keyboard(state.gfx->cam, BACKWARD, delta);
    if (keys[GLFW_KEY_A])
        process_keyboard(state.gfx->cam, LEFT, delta);
    if (keys[GLFW_KEY_D])
        process_keyboard(state.gfx->cam, RIGHT, delta);
    if (keys[GLFW_KEY_LEFT_SHIFT])
        process_keyboard(state.gfx->cam, SHIFT, delta);
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

    process_mouse(state.gfx->cam, xoffset, yoffset, true);
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

void window_init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    window.size = (ivec2s) {{SCREEN_WIDTH, SCREEN_HEIGHT}};
    window.handle = glfwCreateWindow(window.size.x, window.size.y, "Rain", NULL, NULL);
    if (window.handle == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window.handle);

    
    glfwSetCursorPosCallback(window.handle, mouse_callback);
    glfwSetScrollCallback(window.handle, scroll_callback);
    glfwSetKeyCallback(window.handle, key_callback);
    glfwSetFramebufferSizeCallback(window.handle, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
    glfwSwapInterval(1);
}