#include "camera.h"

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

float sprint = 3.0f;
bool fast = false;
Camera *camera_create()
{
    Camera *c = malloc(sizeof(Camera));
    if (c == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    c->pos = VEC3S(0.0f, 0.0f, -3.0f);
    c->up = VEC3S(0.0f, 1.0f, 0.0f);
    c->worldup = c->up;
    c->front = VEC3S(0.0f, 0.0f, -1.0f);
    c->yaw = YAW;
    c->pitch = PITCH;
    c->move_speed = SPEED;
    c->sensitivity = SENSITIVITY;
    c->zoom = ZOOM;

    update_vectors(c);
    return c;
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
mat4s get_view_matrix(Camera *c)
{
    mat4s dest;
    vec3s res = glms_vec3_add(c->pos, c->front);
    glm_lookat(c->pos.raw, res.raw, c->up.raw, dest.raw); // create the view matrix
    return dest;
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void process_keyboard(Camera *c, enum Camera_Movement direction, float deltaTime)
{
    float velocity = (c->move_speed * deltaTime) * (fast ? 3.0f : 1.0f);

    switch (direction)
    {
    case SHIFT:
        fast = !fast;
        break;
    case FORWARD:
        c->pos = glms_vec3_mul(glms_vec3_add(c->pos, glms_vec3_scale(c->front, velocity)), VEC3S(1.0f, 1.0f, 1.0f));
        break;
    case BACKWARD:
        c->pos = glms_vec3_mul(glms_vec3_sub(c->pos, glms_vec3_scale(c->front, velocity)), VEC3S(1.0f, 1.0f, 1.0f));
        break;
    case LEFT:
        c->pos = glms_vec3_sub(c->pos, glms_vec3_scale(c->right, velocity));
        break;
    case RIGHT:
        c->pos = glms_vec3_add(c->pos, glms_vec3_scale(c->right, velocity));
        break;
    }
}
// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void process_mouse(Camera *c, float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= c->sensitivity;
    yoffset *= c->sensitivity;

    c->yaw += xoffset;
    c->pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (c->pitch > 89.0f)
            c->pitch = 89.0f;
        if (c->pitch < -89.0f)
            c->pitch = -88.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_vectors(c);
}
// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void process_scroll(Camera *c, float yoffset)
{
    c->zoom -= (float)yoffset;
    if (c->zoom < 1.0f)
        c->zoom = 1.0f;
    if (c->zoom > 45.0f)
        c->zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void update_vectors(Camera *c)
{
    // calculate the new Front vector
    vec3s front;
    front.x = cos(glm_rad(c->yaw)) * cos(glm_rad(c->pitch));
    front.y = sin(glm_rad(c->pitch));
    front.z = sin(glm_rad(c->yaw)) * cos(glm_rad(c->pitch));
    c->front = glms_normalize(front);

    // also re-calculate the Right and Up vector
    c->right = glms_normalize(glms_cross(c->front, c->worldup)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.

    c->up = glms_normalize(glms_cross(c->right, c->front));
}