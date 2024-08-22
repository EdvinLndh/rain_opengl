#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <stdbool.h>

typedef struct VBO {
    GLuint handle;
    GLint type;
    bool dynamic;
} VBO;

VBO vbo_create(GLint type, bool dynamic);

void vbo_destroy(VBO v);

void vbo_bind(VBO v);

void vbo_buffer(VBO v, void *data, unsigned int size);

#endif