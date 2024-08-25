#include "texture.h"

static Texture2D *create_texture(bool repeat, bool nearest);
static void generate(Texture2D *texture, unsigned int width, unsigned int height, unsigned char *data);

Texture2D *load_texture(const char *path, bool repeat, bool nearest)
{
    Texture2D *texture = create_texture(repeat, nearest);
    if (path == NULL)
    {
        return texture;
    }
    int width, height, nr_ch;

    unsigned char *data = stbi_load(path, &width, &height, &nr_ch, 0);
    if (data == NULL)
    {

        fprintf(stderr, "Failed to load texture %s ", path);
        exit(EXIT_FAILURE);
    }
    else
    {
        GLenum format;
        if (nr_ch == 1)
            format = GL_RED;
        else if (nr_ch == 3)
            format = GL_RGB;
        else if (nr_ch == 4)
            format = GL_RGBA;

        texture->Internal_Format = format;
        texture->Image_Format = format;
    }
    generate(texture, width, height, data);
    stbi_image_free(data);
    return texture;
}

unsigned int load_cubemap(const char **paths)
{

    unsigned int cubeID;
    glGenTextures(1, &cubeID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeID);

    int width, height, nrChannels;
    unsigned char *data;

    for (int i = 0; i < 6; i++)
    {
        data = stbi_load(paths[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            fprintf(stderr, "Cubemap failed to load\n");
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cubeID;
}

TextureSheet *load_texture_sheet(const char *path, int sprite_w, int sprite_h)
{
    TextureSheet *s = malloc(sizeof(TextureSheet));
    s->tex = load_texture(path, false, true);
    s->sprite_w = sprite_w;
    s->sprite_h = sprite_h;

    return s;
}

static void generate(Texture2D *texture, unsigned int width, unsigned int height, unsigned char *data)
{
    texture->Width = width;
    texture->Height = height;

    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->Internal_Format, width, height, 0, texture->Image_Format, GL_UNSIGNED_BYTE, data);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->Filter_Max);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_bind(Texture2D *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}

static Texture2D *create_texture(bool repeat, bool nearest)
{
    Texture2D *texture = malloc(sizeof(Texture2D));
    texture->Width = 0;
    texture->Height = 0;
    texture->Wrap_S = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    texture->Wrap_T = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    texture->Filter_Min = nearest ? GL_NEAREST : GL_LINEAR;
    texture->Filter_Max = nearest ? GL_NEAREST : GL_LINEAR;
    texture->Internal_Format = 0;
    texture->Image_Format = 0;

    glGenTextures(1, &(texture->ID));

    return texture;
}
