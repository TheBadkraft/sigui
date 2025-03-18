// render.c
#include "render.h"
#include "sigui_debug.h"

//	Engine State ================================================================
static SDL_Window* sdl_window = NULL;
static SDL_GLContext gl_context = NULL;
//	[TODO] TASK: expand error codes with string messages
static string ERR_Status = {0};

//	Forward Declarations ========================================================
static int cleanup(const string);

/*
 *	Initializes SDL library and instantiates a sdl_window
 */
static int init_sdl_window(int width, int height) {
	DBLOG("Initializing render engine: width=%d height=%d", width, height);

#ifdef SIMOCK
	//	mocked SDL initialization
	if (SDL_Init(0) != 0) {
		ERR_Status = "SDL_Init";
		return cleanup("[MOCK] SDL init error");
	}
	sdl_window = SDL_CreateWindow("", 0, 0, width, height, 0);
	if(!sdl_window) {
		ERR_Status = "SDL_CreateWindow";
		return cleanup("[MOCK] SDL create window error");
	}
	gl_context = SDL_GL_CreateContext(sdl_window);
	if (!gl_context) {
		ERR_Status = "SDL_GL_CreateContext";
		return cleanup("[V] GL context error");
	}
#else
	//	real SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		ERR_Status = "SDL_Init";
		return cleanup("[V] window creation error");
	}
	
	//	create window
	sdl_window = SDL_CreateWindow(
		"sigui",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_OPENGL
	);
	if(!sdl_window) {
		ERR_Status = "SDL_CreateWindow";
		return cleanup((const string)SDL_GetError());
	}
	
	//	set OpenGL attributes (2.1 context for simplicity)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	//	create OpenGL context
	gl_context = SDL_GL_CreateContext(sdl_window);
	if (!gl_context) {
		ERR_Status = "SDL_GL_CreateContext";
		return cleanup((const string)SDL_GetError());
	}
	
	// Set up OpenGL viewport and projection
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);  // Top-left origin
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background
#endif

	DBLOG("Render engine initialized");
	return 0;
}
/*
 *	Render a module
 */
static void render_module(ui_module m) {
	if (!m || !m->win) {
		DBLOG("No module or window to render");
		return;
	}
	
	window win = m->win;
	DBLOG("Rendering module window at x=%d y=%d w=%d h=%d",
			win->x, win->y, win->width, win->height);
			
#ifdef SIMOCK
	//	mocked rendering
	glClear(0);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(0);		// GL_QUADS
	glVertex2i(win->x, win->y);
	glVertex2i(win->x + win->width, win->y);
	glVertex2i(win->x + win->width, win->y + win->height);
	glVertex2i(win->x, win->y + win->height);
	glEnd();
	SDL_GL_SwapWindow(sdl_window);
#else
	//	real OpenGL rendering
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2i(win->x, win->y);
	glVertex2i(win->x + win->width, win->y);
	glVertex2i(win->x + win->width, win->y + win->height);
	glVertex2i(win->x, win->y + win->height);
	glEnd();
	
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) fprintf(stderr, "GL Error: %d\n", err);
	
	SDL_GL_SwapWindow(sdl_window);
#endif
}
/*
 *	Clean up resources and exit
 *	Pass message if an error condition is causing exit
 */
static int cleanup(const string msg) {
	int ret = msg ? -1 : 0;
	DBLOG("Exiting render engine (%d)", ret);
	if (ret) {
		fprintf(stdout, "Error [%s]: %s\n", ERR_Status, msg);
#ifdef SIMOCK
		// no mock SDL_ClearError
#else
		SDL_ClearError();
#endif
	}
	
	//	dispose context
	if (gl_context) {
		SDL_GL_DeleteContext(gl_context);
		gl_context = NULL;
	}
	//	dispose sdl_window
	if (sdl_window) {
		SDL_DestroyWindow(sdl_window);
		sdl_window = NULL;
	}
	
	SDL_Quit();
	
	DBLOG("Render engine cleanup complete.");
	return ret;
}

const IRender Render = {
    .init = init_sdl_window,
    .module = render_module,
    .dispose = cleanup
};
