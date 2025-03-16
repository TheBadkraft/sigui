#include "sigui.h"
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
	app_state state = {100};
	ui_context ctx = Sigui.new_context(&state);
	if (!ctx) return -1;

	window win = Sigui.new_window(50, 50, 300, 400);
	ui_module m = Sigui.add_module(ctx, "MainWindow", render_window, handle_window_event, win);
	printf("[Main] module=%s\n", m->name);
	
	//	simulate frames w/ input
	ui_input input = {0};
	for (int i = 0; i < FRAME_COUNT; i++) {
		input.mouse_x = i * 10;
		input.mouse_y = i * 20;
		input.button = (i % 2) == 0 ? 1 : 0;
		input.key_space = (i == 3);

		Sigui.render(ctx, &input);
	}

	//	clean up
	Sigui.free_context(ctx);

	return 0;
}

/* Dummy render function */
static void render_window(ui_context ctx, ui_module module, ui_input* input) {
	printf("   <window_module> rendering %s", input ? ": " : "\n");
	if (input){
		printf("input { Mouse (x=%d, y=%d) Button=%d } { Keyboard Space=%d }\n",
			input->mouse_x, input->mouse_y, input->button, input->key_space); 
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

