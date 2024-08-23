#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <cglm/types-struct.h>
#include <string.h>

#include "util.h"

typedef struct Shader
{
    unsigned int ID;
} Shader;

// sets the current shader as active
void shader_use(Shader shader);
// compiles the shader from given source code
Shader shader_compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource); 
// utility functions
void set_float(Shader self, const char *name, float value);
void set_int(Shader self, const char *name, int value);
void set_vec2(Shader self, const char *name, vec2s value);
void set_vec3(Shader self, const char *name, vec3s value);
void set_vec4(Shader self, const char *name, vec4s value);
void set_mat4(Shader self, const char *name, const mat4s matrix);



#endif // !SHADER_H