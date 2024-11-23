#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <cglm/types-struct.h>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct Button {
    bool down, last, last_tick, pressed, pressed_tick;
};

struct Mouse {
    struct Button buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s position, delta;
};

struct Keyboard {
    struct Button keys[GLFW_KEY_LAST];
};


typedef struct Window {
    GLFWwindow *handle;
    ivec2s size;
    struct Mouse mouse;
    struct Keyboard keyboard;

    // timing variables
    // u64 last_second;
    float frames, fps, last_frame, frame_delta;
    float ticks, tps, tick_remainder;
} Window;


void window_init();

extern Window window;

#endif