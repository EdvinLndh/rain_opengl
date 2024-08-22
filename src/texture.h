#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stb_image.h>
#include <string.h>

typedef struct Texture2D {
    unsigned int ID; 
    unsigned int Width, Height; 
    unsigned int Internal_Format; 
    unsigned int Image_Format;
    unsigned int Wrap_S;
    unsigned int Wrap_T;
    unsigned int Filter_Min;
    unsigned int Filter_Max;

} Texture2D; 


typedef struct TextureSheet {
    Texture2D *tex; 
    int sprite_w, sprite_h;
} TextureSheet;

// Texture2D texture_create_from_path(char *path);
  
Texture2D* load_texture(const char *path, bool repeat, bool nearest);
TextureSheet *load_texture_sheet(const char *path, int sprite_w, int sprite_h);

void texture_bind(Texture2D *texture);



#endif // !TEXTURE_H