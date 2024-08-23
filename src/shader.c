#include "shader.h"

static void checkCompileErrors(unsigned int object, const char *type);
static void checkGLError(const char *func);

static char *load(const char *path);

void shader_use(Shader shader)
{
    glUseProgram(shader.ID);
}

Shader shader_compile(const char *vertexSourcePath, const char *fragmentSourcePath, const char *geometrySourcePath)
{
    unsigned int sVertex, sFragment, gShader;
    Shader shader;
    // Vertex Shader
    char *vertexSource = load(vertexSourcePath);
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, (const char *const *)&vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    free(vertexSource); // Free memory after use

    // Fragment Shader
    char *fragmentSource = load(fragmentSourcePath);
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, (const char *const *)&fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    free(fragmentSource); // Free memory after use

    // Geometry Shader
    if (geometrySourcePath != NULL)
    {
        char *geometrySource = load(geometrySourcePath);
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, (const char *const *)&geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
        free(geometrySource); // Free memory after use
    }

    // Shader Program
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, sVertex);
    glAttachShader(shader.ID, sFragment);
    if (geometrySourcePath != NULL)
    {
        glAttachShader(shader.ID, gShader);
    }
    glLinkProgram(shader.ID);
    checkCompileErrors(shader.ID, "PROGRAM");

    // Delete shaders as they're linked into the program now and no longer needed
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySourcePath != NULL)
    {
        glDeleteShader(gShader);
    }
    return shader;
}

static void checkGLError(const char *func)
{
    GLenum err;
    char *err_str;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:
            err_str = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            err_str = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            err_str = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            err_str = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            err_str = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            err_str = "PROBABLY STACK OVERFLOW";
            break;
        }
        fprintf(stderr, "OpenGL error in %s: %s\n", func, err_str);
    }
}

void set_float(Shader self, const char *name, float value)
{
    GLint location = glGetUniformLocation(self.ID, name);
    if (location == -1)
    {
        fprintf(stderr, "Warning: Uniform '%s' not found in shader program %u\n", name, self.ID);
    }
    else
    {
        glUniform1f(location, value);
        checkGLError("set_float");
    }
}

void set_int(Shader self, const char *name, int value)
{
    GLint location = glGetUniformLocation(self.ID, name);
    if (location == -1)
    {
        fprintf(stderr, "Warning: Uniform '%s' not found in shader program %u\n", name, self.ID);
    }
    else
    {
        glUniform1i(location, value);
        checkGLError("set_int");
    }
}

void set_vec2(Shader self, const char *name, vec2s value)
{
    GLint location = glGetUniformLocation(self.ID, name);
    if (location == -1)
    {
        fprintf(stderr, "Warning: Uniform '%s' not found in shader program %u\n", name, self.ID);
    }
    else
    {
        glUniform2f(location, value.raw[0], value.raw[1]);
        checkGLError("set_vec2");
    }
}

void set_vec3(Shader self, const char *name, vec3s value)
{
    GLint location = glGetUniformLocation(self.ID, name);
    if (location == -1)
    {
        fprintf(stderr, "Warning: Uniform '%s' not found in shader program %u\n", name, self.ID);
    }
    else
    {
        glUniform3f(location, value.raw[0], value.raw[1], value.raw[2]);
        checkGLError("set_vec3");
    }
}

void set_vec4(Shader self, const char *name, vec4s value)
{
    GLint location = glGetUniformLocation(self.ID, name);
    if (location == -1)
    {
        fprintf(stderr, "Warning: Uniform '%s' not found in shader program %u\n", name, self.ID);
    }
    else
    {
        glUniform4f(location, value.raw[0], value.raw[1], value.raw[2], value.raw[3]);
        checkGLError("set_vec4");
    }
}

void set_mat4(Shader self, const char *name, const mat4s matrix)
{
    GLint location = glGetUniformLocation(self.ID, name);
    if (location == -1)
    {
        fprintf(stderr, "Warning: Uniform '%s' not found in shader program %u\n", name, self.ID);
    }
    else
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat *)&matrix.raw);
        checkGLError("set_mat4");
    }
}

static void checkCompileErrors(unsigned int object, const char *type)
{
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            printf("| ERROR::SHADER: Compile-time error: Type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            printf("| ERROR::Shader: Link-time error: Type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
}

static char *load(const char *path)
{
    FILE *f;
    char *text;
    long len;

    f = fopen(path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "error loading shader at %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);

    fseek(f, 0, SEEK_SET);
    text = calloc(1, len + 1);
    fread(text, 1, len, f);
    fclose(f);
    assert(strlen(text) > 0);

    return text;
}