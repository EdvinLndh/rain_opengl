#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <cglm/types-struct.h>
#include <stdlib.h>
#include <glad/glad.h>

#include "util.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    SHIFT
};



typedef struct Camera
{
    vec3s pos;
    vec3s front;
    vec3s up;
    vec3s right;
    vec3s worldup;
    float yaw;
    float pitch;
    float move_speed;
    float sensitivity;
    float zoom;
} Camera;

Camera *camera_create();
mat4s get_view_matrix(Camera *c);
void process_keyboard(Camera *c, enum Camera_Movement direction, float deltaTime);
void process_mouse(Camera *c, float xoffset, float yoffset, GLboolean constrainPitch);
void process_scroll(Camera *c, float yoffset);
void update_vectors(Camera *c);

#endif