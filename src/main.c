#include "sigui.h"
#include "render.h"
#include "ui_core.h"

const int FRAME_COUNT = 5;

typedef struct {
	int value;
} app_state;

static void render_window(ui_context, ui_module, ui_input*);
static void handle_window_event(ui_context, ui_module, event_info);
static void execute_show_message(ui_context, ui_module);
static void execute_toggle_state(ui_context, ui_module);

int main(void) {
	if (Render.init(800, 600) != 0) {
		fprintf(stderr, "failed to initialize rendering engine\n");
		return -1;
	}
	
	app_state state = {100};
	ui_context ctx = Sigui.new_context(&state);
	if (!ctx) return -1;

	window win = Sigui.new_window(50, 50, 300, 400);
	ui_module m = Sigui.add_module(ctx, "MainWindow", render_window, handle_window_event, win);
	printf("[Main] module=%s\n", m->name);
	
	//	main loop
	int running = 1;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) running = 0;
		}
		
		Render.module(m);
	}
	
	//	clean up
	Render.dispose(NULL);
	Sigui.free_context(ctx);

	return 0;
}

/* Dummy render function */
static void render_window(ui_context ctx, ui_module module, ui_input* input) {
	printf("   <window_module> rendering %s", input ? ": " : "\n");
	if (input){
		printf("input { Mouse (x=%d, y=%d) Button=%d } { Keyboard Space=%d }\n",
			input->mouse_x, input->mouse_y, input->button, input->keys[' ']); 
	}
}
static void handle_window_event(ui_context ctx, ui_module module, event_info ei) {
	if (!ei) return;
	event e = ei->e;
	
	if (e->type == EVENT_MOUSE_PRESS) {
		printf("    <window_module> Mouse pressed at (%d, %d)\n", e->data.mouse.x, e->data.mouse.y);
		command cmd = Sigui.new_command("show_message");
		if (cmd) {
			cmd->target = module; 									// the target module
			cmd->execute = execute_show_message;
			
			Dispatcher.queue_command(ctx, cmd);
		}
	} else if (e->type == EVENT_KEY_PRESS && e->data.key.key_code == ' ') {
	  printf("    <window_module> Space key pressed\n");
	  command cmd = Sigui.new_command("toggle_state");
	  if (cmd) {
			cmd->target = module; 									// the target module
			cmd->execute = execute_toggle_state;

			Dispatcher.queue_command(ctx, cmd);
	  }
	}
}
static void execute_show_message(ui_context ctx, ui_module m) {
    printf("    <command> Showing message from module %s\n", m->name);
}

static void execute_toggle_state(ui_context ctx, ui_module m) {
    app_state* state = (app_state*)ctx->state;
    state->value = !state->value;
    printf("    <command> Toggled state to %d\n", state->value);
}

