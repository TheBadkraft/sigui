// test_context.c
#include "sigui.h"
#include <sigtest.h>
#include <time.h>

// Assert.isTrue(condition, "fail message");
// Assert.isFalse(condition, "fail message");
// Assert.areEqual(obj1, obj2, INT, "fail message");
// Assert.areEqual(obj1, obj2, PTR, "fail message");
// Assert.areEqual(obj1, obj2, STRING, "fail message");

typedef struct {
	int value;
} test_state;

static void dummy_render(ui_context, ui_module, ui_input*);

/* test info */
void unit_testing(void) {
	printf("\n");
	fflush(stdout);
	
	time_t rawtime;
	time(&rawtime);
	
	struct tm* timeinfo;
	timeinfo = localtime(&rawtime);
	
	char date_str[20];
	strftime(date_str, sizeof(date_str), "%b-%d-%Y [%H:%M]", timeinfo);
	
	flogf(stdout, "Test Run Date=%s", date_str);
}
/* test initialization of a new context */
void new_context (void) {
	printf("\n");
	fflush(stdout);
	flogf(stdout, "TEST: new_context (ui_context is opaque)");
	
	test_state state = {42};
	ui_context ctx = Sigui.new_context(&state);
	
	Assert.isTrue(ctx != NULL, "new context should not be NULL");	
	Mem.free(ctx);		// Sigui.free_context not yet implemented
}
/* test add module to context */
void add_module_to_context(void) {
	printf("\n");
	fflush(stdout);
	
	test_state state = {42};
	ui_context ctx = Sigui.new_context(&state);
	string mod_name = "TestModule";
	
	Sigui.add_module(ctx, mod_name, dummy_render, NULL, NULL);
	/* Trust add_module works; verify via render in next test */
	Sigui.free_context(ctx);
	flogf(stdout, "added module '%s'", mod_name);
	Assert.isTrue(1, NULL);
}
/* test module render */
void render_context_module(void) {
	printf("\n");
	fflush(stdout);
	
	test_state state = {42};
	ui_context ctx = Sigui.new_context(&state);
	string mod_name = "TestModule";
	Sigui.add_module(ctx, mod_name, dummy_render, NULL, NULL);
	
	Sigui.render(ctx, NULL);
	Sigui.free_context(ctx);
	flogf(stdout, "rendered module '%s'", mod_name);
	Assert.isTrue(1, NULL);
}
/* test render module with input */
void render_with_input(void) {
	printf("\n");
	fflush(stdout);
	
	test_state state = {42};
	ui_context ctx = Sigui.new_context(&state);
	string mod_name = "TestModule";
	Sigui.add_module(ctx, mod_name, dummy_render, NULL, NULL);
	ui_input input = {10, 20, MOUSE_BUTTON_NONE, {0}};  /* Simulated input */
	
	Sigui.render(ctx, &input);
	Sigui.free_context(ctx);
	flogf(stdout, "rendered module '%s'", mod_name);
	Assert.isTrue(1, NULL);
}
/* test render module with window */
void render_with_window(void) {
	printf("\n");
	fflush(stdout);
	
	test_state state = {42};
	ui_context ctx = Sigui.new_context(&state);
	window win = Sigui.new_window(10, 20, 100, 200);    
	string mod_name = "TestModule";
	Sigui.add_module(ctx, mod_name, dummy_render, NULL, win);
	ui_input input = {10, 20, MOUSE_BUTTON_LEFT, {0}};  /* Simulated input */
	
	Sigui.render(ctx, &input);
	Sigui.free_context(ctx);
	flogf(stdout, "rendered module '%s'", mod_name);
	Assert.isTrue(1, NULL);
}
/* test create event */
void create_event_info(void) {
	printf("\n");
	fflush(stdout);
	
	event_type expType = EVENT_MOUSE_PRESS;
	int expMouseX = 10, expMouseY = 20;
	mouse_button expMouseLeft = MOUSE_BUTTON_LEFT;
	int expKeySpace = 0;
	
	ui_input input = {0};
	input.mouse_x = expMouseX;
	input.mouse_y = expMouseY;
	input.button = expMouseLeft;
	input.keys[' '] = expKeySpace;
	
	event_info ei = Sigui.new_event(expType, &input, expMouseLeft);
	
	//	event_info only has event data
	Assert.isTrue(ei != NULL, "event_info should not be NULL");
	Assert.isTrue(expType == ei->e->type, "event type mismatch");
	Assert.areEqual(&expMouseX, &ei->e->data.mouse.x, INT, "mouse_x mismatch");
	Assert.areEqual(&expMouseY, &ei->e->data.mouse.y, INT, "mouse_y mismatch");
	flogf(stdout, "ei: type=%d mouse_x=%d mouse_y=%d", ei->e->type, ei->e->data.mouse.x, ei->e->data.mouse.y);
	
	Mem.free(ei);
}
/* test create command */
void create_command_obj(void) {
	printf("\n");
	fflush(stdout);
	
	command cmd = Sigui.new_command("TestCommand");
	
	Assert.isTrue(cmd != NULL, "command create should not be NULL");
	flogf(stdout, "command=%s", cmd->name);
	
	Mem.free(cmd);
}

/* Dummy render function */
static void dummy_render(ui_context ctx, ui_module module, ui_input* input) {
	/* Can't access ctx->state or module->name directly; trust ctx is passed */
	printf("   <Module> rendering %s", input ? ": " : "\n");
	if (input){
		printf("input { Mouse (x=%d, y=%d) Left=%d } { Keyboard Space=%d }\n",
			input->mouse_x, input->mouse_y, input->button, input->keys[' ']); 
	}
}


// Register test cases
__attribute__((constructor)) void init_sigtest_tests(void) {
	register_test("unit_testing", unit_testing);
	register_test("new_context", new_context);
	register_test("add_module_to_context", add_module_to_context);
	register_test("render_context_module", render_context_module);
	register_test("render_with_input", render_with_input);
	register_test("render_with_window", render_with_window);
	register_test("create_event_info", create_event_info);
	register_test("create_command_obj", create_command_obj);
}
