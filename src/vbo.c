#include "vbo.h"

VBO vbo_create(GLint type, bool dynamic) {
    VBO v = {
        .type = type,
        .dynamic = dynamic
    };
    glGenBuffers(1, &v.handle);
    return v;
}

void vbo_destroy(VBO v) {
    glDeleteBuffers(1, &v.handle);
}

void vbo_bind(VBO v) {
    glBindBuffer(v.type, v.handle);
}

void vbo_buffer(VBO v, void *data, unsigned int size) {
    vbo_bind(v);
    glBufferData(v.type, size, data, v.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glBindBuffer(v.type, 0);
}