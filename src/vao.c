#include "vao.h"

VAO vao_create() {
    VAO v;
    glGenVertexArrays(1, &v.handle);
    return v;
}

void vao_destroy(VAO v) {
    glDeleteVertexArrays(1, &v.handle);
}

void vao_bind(VAO v) {
    glBindVertexArray(v.handle);
}

void vao_attr(
    VAO vao, VBO vbo, GLuint index, GLint size, GLenum type,
    GLsizei stride, unsigned long offset) {
    vao_bind(vao);
    vbo_bind(vbo);

    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            glVertexAttribIPointer(index, size, type, stride, (void *) offset);
            break;
        default:
            glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *) offset);
            break;
    }
    glEnableVertexAttribArray(index);

    glBindVertexArray(0);
}