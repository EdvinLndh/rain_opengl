#ifndef IMGUI_H
#define IMGUI_H

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "particle.h"

typedef void (*ButtonCallback)(void *data);

void imgui_init(GLFWwindow *window, const char *glsl_version);
void imgui_start_frame();
void imgui_render(GLFWwindow *window);
void imgui_button(const char *label, float x, float y, void *data, ButtonCallback callback);
void imgui_treenode_particle(char *title, int num_particles, float rainx, float rainy, float rainz, vec4s color, Shader shader, ButtonCallback func);
void imgui_cleanup();

#endif // !IMGUI_H