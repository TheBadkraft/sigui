# sigui - Structured Immediate-mode GUI  
A lightweight, immediate-mode GUI library built in C, designed for simplicity and extensibility. `sigui` uses an event-driven architecture with modules, commands, and a flexible rendering pipeline, currently transitioning from console output to an OpenGL backend.

## Features
- Immediate-mode rendering: UI redraws each frame based on input state.
- Event/command system: Handles mouse/keyboard input and actions.
- Modular design: Add custom modules with render and event handlers.
- Current Backend: Console (Sprint 5); OpenGL with SDL in progress (Sprint 6).

## Getting Started  
### Prerequisites  
- C compiler (e.g., `gcc`)
- [`sigcore` library][1] (custom memory/list/string utilities)
- **SDL2** (for Sprint 6+ OpenGL backend): `sudo apt install libsdl2-dev` (Linux) or equivalent
- **OpenGL** (included with SDL2 or OS)

### Build  
``` bash
make               # Builds the library
make main          # Builds the library and main executable
make test_<name>   # Runs unit tests (test_context, test_dispatcher, etc.)
make clean         # Cleans build artifacts
```

### Run  
``` bash
./bin/main         # Runs main.c (console output until Sprint 6 complete)
```

### Project Structure  
- `src/`:     Core source files(`sigui.c`, `dispatcher.c`, `render.c` [**Sprint 6**])
- `include/`: Headers(`sigui.h`, *`sigui_test.h*`* [**Sprint 6**])
- `test/`:    Unit tests(`test_context.c`, `test_dispatcher.c`, `test_rendering.c` [**Sprint 6**])
- `main.c`:   Functional demo (evolving into **SDL** + **OpenGL** window [**Sprint 6**])

### Status  
See [Current Status][2] for sprint progress and task list.  
Also, check out the [To-Do List][3] for ways to get involved.  

At the completion of *Sprint 6*, this is what our `main.c` looks like:  
``` c
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

  //  generic state object can be access or modified in event handlers
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
//  this could be an empty function ... we are not actually processing events with widgets
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
```
Result of *Sprint 6*  
[![Black w/ white quad](https://github.com/TheBadkraft/sigui/blob/main/docs/images/sigui_window.png)]  

### Contributing  
- Define test cases before coding (see *[Current Status]*[2] for process).
- Submit pull requests with tests and documentation updates prior to adding features, enhancements, etc.

### License  
[GNU GENERAL PUBLIC LICENSE][3]
- Version 3, 29 June 2007

[1]: https://github.com/TheBadkraft/sigcore  
[2]: https://github.com/TheBadkraft/sigui/blob/main/docs/current_status.md  
[3]: https://github.com/TheBadkraft/sigui/blob/main/docs/to_do_list.md  
[4]: https://github.com/TheBadkraft/sigui/blob/main/LICENSE
