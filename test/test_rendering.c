// test_rendering.c
#include "sigui.h"
#include "../src/ui_core.h"
#include "render.h"
#include "sigui_debug.h"
#include <sigtest.h>
#include <sigcore.h>
#include <string.h>
#include <time.h>

#define POKE 0

// Assert.isTrue(condition, "fail message");
// Assert.isFalse(condition, "fail message");
// Assert.areEqual(obj1, obj2, INT, "fail message");
// Assert.areEqual(obj1, obj2, PTR, "fail message");
// Assert.areEqual(obj1, obj2, STRING, "fail message");

static void test_dummy_renderer(ui_context, ui_module, ui_input*);

static void reset_mocks(void);

/* test info */
void test_harness(void) {
	printf("\n");
	fflush(stdout);
	
	time_t rawtime;
	time(&rawtime);
	
	struct tm* timeinfo;
	timeinfo = localtime(&rawtime);
	
	char date_str[20];
	strftime(date_str, sizeof(date_str), "%b-%d-%Y [%H:%M]", timeinfo);
	flogf(stdout, "Configuring sigui debugging ...");	
	
	string is_sidbug;
	#ifdef SIDBUG
	is_sidbug = "TRUE";
	#else
	is_sidbug = "FALSE";
	#endif
	flogf(stdout, " -SIDBUG defined=%s", is_sidbug);
	
	string is_test;
	#ifdef SIMOCK
	is_test = "TRUE";
	#else
	is_test = "FALSE";
	#endif
	flogf(stdout, " -SIMOCK defined=%s", is_test);
	
	DBLOG("Begin Mock Tests Date=%s", date_str);
	
	if (POKE) {
		// poke mocks
		SDL_Init(0);
		DBLOG("SDL Init called; mock counter=%d", mock_sdl_init_called);
		Assert.isTrue(mock_sdl_init_called == 1, "(mock)SDL_Init should be called once");
		
		glBegin(0);
		DBLOG("glBegin called; mock counter=%d", mock_gl_begin_called);
		Assert.isTrue(mock_gl_begin_called == 1, "(mock)glBegin should be called once");
		
		glEnd();
		DBLOG("glEnd called; mock counter=%d", mock_gl_end_called);
		Assert.isTrue(mock_gl_end_called == 1, "(mock)glEnd should be called once");
	}
	
	reset_mocks();
}
/* render engine init */
void render_engine_init(void) {
	printf("\n");
	fflush(stdout);
	flogf(stdout, "rendering engine initialization test");
	reset_mocks();
	
	int res = Render.init(800, 600);
	Assert.isTrue(res == 0, "Render.init should return 0 on success.");
	Assert.isTrue(mock_sdl_init_called == 1, "SDL_Init should be called once");
	Assert.isTrue(mock_sdl_window_created == 1, "SDL_CreateWindow should be called once");
	Assert.isTrue(mock_gl_context_created == 1, "SDL_GL_CreateContext should be called once");

	Render.dispose(NULL);
	reset_mocks();
}
/* render engine cleanup */
void test_render_engine_cleanup(void) {
	printf("\n");
	fflush(stdout);
	flogf(stdout, "rendering engine cleanup test");
	reset_mocks();

	Render.init(800, 600);
	Render.dispose(NULL);

	Assert.isTrue(mock_sdl_init_called == 1, "SDL_Init was called during setup");
	Assert.isTrue(mock_sdl_window_created == 1, "SDL_CreateWindow was called during setup");
	Assert.isTrue(mock_gl_context_created == 1, "SDL_GL_CreateContext was called during setup");

	reset_mocks();
}
/* render engine render module */
void test_render_module(void) {
	printf("\n");
	fflush(stdout);
	flogf(stdout, "rendering module");
	reset_mocks();

	ui_context ctx = Sigui.new_context(NULL);
	Assert.isTrue(ctx != NULL, "context creation failed");
	window win = Sigui.new_window(50, 50, 300, 400);
	Assert.isTrue(win != NULL, "window creation failed");
	//	must have a renderer to create module
	ui_module m = Sigui.add_module(ctx, "TestModule", test_dummy_renderer, NULL, win);
	Assert.isTrue(m != NULL, "module creation failed");
	flogf(stdout, "module=%s window=%s", m->name, m->win ? "TRUE" : "FALSE");
	
	Render.init(800, 600);
	Render.module(m);

	Assert.isTrue(mock_gl_begin_called == 1, "glBegin should be called once");
	Assert.isTrue(mock_gl_end_called == 1, "glEnd should be called once");

	Render.dispose(NULL);
	Sigui.free_context(ctx);
	reset_mocks();
}

static void test_dummy_renderer(ui_context ctx, ui_module m, ui_input* input) {
	// ... dummy renderer
}

/* reset mock counters */
static void reset_mocks(void) {
	mock_sdl_init_called = 0;
	mock_sdl_window_created = 0;
	mock_gl_context_created = 0;
	mock_gl_begin_called = 0;
	mock_gl_end_called = 0;
}

// Register test cases
__attribute__((constructor)) void init_sigtest_tests(void) {
	register_test("test_harness", test_harness);
	register_test("render_engine_init", render_engine_init);
	register_test("test_render_engine_cleanup", test_render_engine_cleanup);
	register_test("test_render_module", test_render_module);
}
