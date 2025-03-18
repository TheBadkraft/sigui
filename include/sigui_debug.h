// sigui_debug.h
#ifndef SIGUI_DEBUG_H
#define SIGUI_DEBUG_H

#include <stdint.h>
#include <sigcore.h>

// Debug logging
#ifdef SIDBUG

#define DBLOG(fmt, ...) fprintf(stdout, "[DBLOG] " fmt "\n", ##__VA_ARGS__); \
		  fflush(stdout)
#else
#define DBLOG(fmt, ...)
#endif	// SIDBUG

// Mock SDL/OpenGL for tests
#ifdef SIMOCK
#include <stddef.h> // For NULL

typedef struct { int dummy; } SDL_Window;
typedef object SDL_GLContext;

SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, uint32_t flags);
void SDL_DestroyWindow(SDL_Window* window);
SDL_GLContext SDL_GL_CreateContext(SDL_Window* window);
void SDL_GL_DeleteContext(SDL_GLContext context);
void SDL_GL_SwapWindow(SDL_Window* window);
int SDL_Init(uint32_t flags);
void SDL_Quit(void);

void glClear(uint32_t mask);
void glBegin(uint32_t mode);
void glVertex2i(int x, int y);
void glEnd(void);
void glColor3f(float r, float g, float b);

extern int mock_sdl_init_called;
extern int mock_sdl_window_created;
extern int mock_gl_context_created;
extern int mock_gl_begin_called;
extern int mock_gl_end_called;
#endif // SIMOCK

#endif // SIGUI_DEBUG_H
