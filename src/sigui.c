// sigui.c
/** 
 * @detail Here we can add a lot of detail about what is going on from a 30,000 foot perspective. 
 */
 
#include "sigui.h"
#include "ui_core.h"
 
static ui_input INIT_INPUT = {0};

//	Private structs =============================================================
typedef struct input_delta_s {
	// [TODO] TASK: determine if this will benefit from mask values
	uint32_t mouse_button_pressed;	// bitmask of pressed buttons
	uint32_t mouse_button_released;	//	bitmask of release buttons
	//	[TODO] TASK: update keyboard input delta (masking ???)
	uint8_t key_pressed[256];			// array of pressed keys
	uint8_t key_released[256];			//	array of released keys
} input_delta;

//	Helper Functions ============================================================
static void generate_events(ui_context, ui_input*);
static input_delta compute_input_delta(ui_input*, ui_input*);
static event_info create_event(event_type, ui_input*, uint32_t);

/* creates a new sigui context */
static ui_context new_ui_context(object state) {
	ui_context ctx = Mem.alloc(sizeof(struct sigui_context_s));
	if (!ctx) return NULL;
	
	ctx->modules = List.new(4);	/* initialize modules capacity -> 4 */
	ctx->events = List.new(4);		/* initialize event queue */
	ctx->commands = List.new(4);	/* initialize the command queue */
	if (!ctx->modules || !ctx->events || !ctx->commands) {
		Mem.free(ctx);
		return NULL;
	}
	
	ctx->state = state;
	ctx->input_state = INIT_INPUT;	/* initialize last input */
	return ctx;
}
/* creates a new window */
static window new_ui_window(int x, int y, int w, int h) {
	window win = Mem.alloc(sizeof(struct ui_window_s));
	if (!win) return NULL;
	
	win->x = x;
	win->y = y;
	win->width = w;
	win->height = h;
	
	return win;
}
/* add a module to the context */
static ui_module add_module(ui_context ctx, string name, ui_render renderer, event_handler h, window win) {
	if (!ctx || !name || !renderer) return NULL;
	
	ui_module m = Mem.alloc(sizeof(struct sigui_module_s));
	if (!m) return NULL;
	
	m->name = String.copy(name);
	m->render = renderer;
	m->handler = h;
	m->enabled = 1;
	m->win = win;
	
	List.add(ctx->modules, m);
	
	return m;
}
/* renders all enabled modules */
static void render_ui(ui_context ctx, ui_input* input) {
	if (!ctx) return;
	if (!ctx->modules || List.count(ctx->modules) == 0) return;
	printf("--- Frame Begin ---\n");
	
	generate_events(ctx, input);			//	generate ui events
	Dispatcher.dispatch_events(ctx);		// dispatch all events
	Dispatcher.dispatch_commands(ctx);	//	dispatch all commands
	
	//	render context modules
	iterator it = Array.getIterator(ctx->modules, LIST);
	while (Iterator.hasNext(it)) {
		ui_module m = Iterator.next(it);
		if (m->enabled && m->render) {
			printf("   Rendering module: %s\n    ", m->name);  /* Show name */
			m->render(ctx, m, input);
		}
	}
	Iterator.free(it);
	
	printf("--- Frame End ---\n");
	
}
/* frees a sigui context */
static void free_ui_context(ui_context ctx) {
	if (!ctx) return;
	
	//	free command queue & commands
	if (ctx->commands && List.count(ctx->commands) > 0) {
		iterator it = Array.getIterator(ctx->commands, LIST);
		while (Iterator.hasNext(it)) {
			command c = Iterator.next(it);
			if (c->name) String.free(c->name);
			Mem.free(c);
		}
		Iterator.free(it);
		List.free(ctx->commands);
	}
	//	free event queue & events
	if (ctx->events && List.count(ctx->events) > 0) {	
		iterator it = Array.getIterator(ctx->events, LIST);
		while (Iterator.hasNext(it)) {
			event_info ei = Iterator.next(it);
			Mem.free(ei->e);
			Mem.free(ei);
		}
		Iterator.free(it);
		List.free(ctx->events);
	}
	//	free modules
	if (ctx->modules && List.count(ctx->modules) > 0) {
		iterator it = Array.getIterator(ctx->modules, LIST);
		while (Iterator.hasNext(it)) {
			ui_module m = Iterator.next(it);
			if (m->name) String.free(m->name);
			if (m->win) Mem.free(m->win);
			
			Mem.free(m);
		}
		Iterator.free(it);
		List.free(ctx->modules);
	}
	
	Mem.free(ctx);
}

/* event factory */
static event_info create_event(event_type type, ui_input* input, uint32_t value) {
	event e = Mem.alloc(sizeof(struct event_s));
	if (!e) return NULL;
	
	e->type = type;
	
	switch (type) {
		case EVENT_MOUSE_PRESS:
		case EVENT_MOUSE_RELEASE:
			e->data.mouse.x = input->mouse_x;
			e->data.mouse.y = input->mouse_y;
			e->data.mouse.button = value;
		
			break;
		case EVENT_KEY_PRESS:
		case EVENT_KEY_RELEASE:
			e->data.key.key_code = (int)value;
			
			break;
		default:
			Mem.free(e);
			return NULL;
	}
	
	event_info ei = Mem.alloc(sizeof(struct event_info_s));
	if (!ei) {
		Mem.free(e);
		return NULL;
	}
	
	ei->e = e;
	return ei;
}
/* command factory */
static command create_command(const string name) {
	command cmd = Mem.alloc(sizeof(struct command_s));
	if (!cmd) return NULL;
	
	cmd->name = String.copy(name);
	
	return cmd;
}
/* generate input events */
static void generate_events(ui_context ctx, ui_input* input) {
	if (!ctx || !input) return;
	
	//	[TODO] TASK: handle mouse move
	//	[TODO] TASK; handle mouse wheel click (might be one of the button #s
	//	[TODO] TASK: handle mouse wheel scroll
	//	[TODO] TASK: ensure test case handles a modifer + mouse_button (SHIFT + click)
	
	//	use the previous state before overwriting
	ui_input prev_input = ctx->input_state;
	input_delta delta = compute_input_delta(input, &prev_input);
	
	//	mouse events
	if (delta.mouse_button_pressed & MOUSE_BUTTON_LEFT) {
		event_info ei = create_event(EVENT_MOUSE_PRESS, input, MOUSE_BUTTON_LEFT);
		if (ei) {
			ei->e->data.mouse.button = MOUSE_BUTTON_LEFT;
			Dispatcher.queue_event(ctx, ei);
		}
	}
	if (delta.mouse_button_released & MOUSE_BUTTON_LEFT) {
		event_info ei = create_event(EVENT_MOUSE_RELEASE, input, MOUSE_BUTTON_LEFT);
		if (ei) {
			ei->e->data.mouse.button = MOUSE_BUTTON_LEFT;
			Dispatcher.queue_event(ctx, ei);
		}
	}
	if (delta.mouse_button_pressed & MOUSE_BUTTON_RIGHT) {
		event_info ei = create_event(EVENT_MOUSE_PRESS, input, MOUSE_BUTTON_RIGHT);
		if (ei) {
			ei->e->data.mouse.button = MOUSE_BUTTON_RIGHT;
			Dispatcher.queue_event(ctx, ei);
		}
	}
	if (delta.mouse_button_released & MOUSE_BUTTON_RIGHT) {
		event_info ei = create_event(EVENT_MOUSE_RELEASE, input, MOUSE_BUTTON_RIGHT);
		if (ei) {
			ei->e->data.mouse.button = MOUSE_BUTTON_RIGHT;
			Dispatcher.queue_event(ctx, ei);
		}
	}
	/*	add more mouse buttons/wheel/move as needed */
	
	//	key events
	int i = 0;
	while (i < sizeof(INIT_INPUT.keys)) {
		if (delta.key_pressed[i]) {
			event_info ei = create_event(EVENT_KEY_PRESS, input, i);
			if (ei) Dispatcher.queue_event(ctx, ei);
		}
		if (delta.key_released[i]) {
			event_info ei = create_event(EVENT_KEY_RELEASE, input, i);
			if (ei) Dispatcher.queue_event(ctx, ei);
		}
		
		++i;
	}
	
	ctx->input_state = *input;
}
/* compute input delta helper */
static input_delta compute_input_delta(ui_input* current, ui_input* prev) {
	// [TODO] TASK: to be enhanced when we implement masking for multiple keys + modifiets (SHIFT/ALT,CTRL)
	input_delta delta = {0};
	
	//	mouse button transitions
	delta.mouse_button_pressed = current->button & ~prev->button;
	delta.mouse_button_released = ~current->button & prev->button;
	
	//	key transitions
	int i = 0;
	while (i < sizeof(INIT_INPUT.keys)) {
		delta.key_pressed[i] = current->keys[i] && !prev->keys[i];
		delta.key_released[i] = !current->keys[i] && prev->keys[i];
	
		i++;
	}
	
	return delta;
}

/* sigui interface */
const ISigui Sigui = {
	.new_context = new_ui_context,
	.free_context = free_ui_context,
	.new_window = new_ui_window,
	.add_module = add_module,
	.render = render_ui,
	.new_event = create_event,
	.new_command = create_command
};
