#ifndef UTIL_H
#define UTIL_H
#define VEC2S(x, y) ((vec2s){{x, y}})
#define VEC3S(x, y, z) ((vec3s){{x, y, z}})
#define VEC4S(x, y, z, a) ((vec4s) {{x, y, z, a}})
#define RANDOM_COLOR() \
    (vec4s) {{ (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.0f }}
#endif