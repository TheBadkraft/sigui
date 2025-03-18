// render.h
#ifndef RENDER_H
#define RENDER_H

#include "../src/ui_core.h"
#include <stdint.h>
#include <sigcore.h>

// Use real SDL/OpenGL unless SIMOCK is defined
#ifndef SIMOCK
#include <SDL2/SDL.h>
#include <GL/gl.h>
#else
#include "sigui_debug.h"
#endif

/** @brief Render interface */
typedef struct IRender {
    int (*init)(int, int);
    void (*module)(ui_module);
    int (*dispose)(const string);
} IRender;

extern const IRender Render;

#endif // RENDER_H
