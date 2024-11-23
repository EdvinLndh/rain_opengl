#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct Global Global; 

#include "gfx.h"
#include "window.h"

typedef struct Global {
    GfxSystem *gfx;
    Window *window;
} Global;

extern Global state;

#endif // !GLOBAL_H