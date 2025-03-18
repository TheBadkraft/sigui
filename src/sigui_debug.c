// sigui_debug.c (optional, link with tests)
#ifdef SIMOCK
#include "sigui_debug.h"

int mock_sdl_init_called = 0;
int mock_sdl_window_created = 0;
int mock_gl_context_created = 0;
int mock_gl_begin_called = 0;
int mock_gl_end_called = 0;

SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, uint32_t flags) {
	mock_sdl_window_created++;
	return (SDL_Window*)1; // Non-NULL dummy
}
void SDL_DestroyWindow(SDL_Window* window) {}
SDL_GLContext SDL_GL_CreateContext(SDL_Window* window) {
	mock_gl_context_created++;
	return (SDL_GLContext)1;
}
void SDL_GL_DeleteContext(SDL_GLContext context) {}
void SDL_GL_SwapWindow(SDL_Window* window) {}
int SDL_Init(uint32_t flags) {
	mock_sdl_init_called++;
	return 0;
}
void SDL_Quit(void) {}

void glClear(uint32_t mask) {}
void glBegin(uint32_t mode) { mock_gl_begin_called++; }
void glVertex2i(int x, int y) {}
void glEnd(void) { mock_gl_end_called++; }
void glColor3f(float r, float g, float b) {}

#endif // SIMOCK
