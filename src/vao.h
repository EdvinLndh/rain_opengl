#ifndef VAO_H
#define VAO_H

#include "vbo.h"

typedef struct VAO
{
    GLuint handle;
} VAO;

VAO vao_create();
void vao_destroy(VAO v);
void vao_bind(VAO v);
void vao_attr(
    VAO vao, VBO vbo, GLuint index, GLint size, GLenum type,
    GLsizei stride, unsigned long offset);

#endif