// test_dispatcher.c
#include "sigui.h"
#include "../src/ui_core.h"
#include <sigtest.h>
#include <sigcore.h>
#include <string.h>
#include <time.h>

// Assert.isTrue(condition, "fail message");
// Assert.isFalse(condition, "fail message");
// Assert.areEqual(obj1, obj2, INT, "fail message");
// Assert.areEqual(obj1, obj2, PTR, "fail message");
// Assert.areEqual(obj1, obj2, STRING, "fail message");

static int event_handled = 0;
static int event_counts[EVENT_KEY_RELEASE + 1] = {0}; // Track event types

//	dummy renderer function for module
static void dummy_render(ui_context, ui_module, ui_input*);
//	event handler
static void test_handler(ui_context, ui_module, event_info);
//	event transition handler
static void test_transition_handler(ui_context, ui_module, event_info);
//	command executor
static void test_command_execute(ui_context, ui_module);
//	command handler
static void test_command_handler(ui_context, ui_module, event_info);
//	create a mouse event_info object
static event_info create_mouse_event(event_type, int, int, int, ui_input*);
//	create a keyboard event_info object
static event_info create_keyboard_event(event_type, int, ui_input*);
//	reset event counts
static void reset_event_counts(void);

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
/* test event queueing */
void queue_ui_event(void) {
	printf("\n");
	fflush(stdout);
	
	ui_context ctx = Sigui.new_context(NULL);
	Assert.isTrue(ctx != NULL, "context should not be NULL");
	
	ui_input input = {0};
	int expMouseX = 10, expMouseY = 20;
	mouse_button expMouseLeft = MOUSE_BUTTON_LEFT;
	//int expKeySpace = 0;
	event_type expType = EVENT_MOUSE_PRESS;
	
	event_info ei = create_mouse_event(expType, expMouseX, expMouseY, expMouseLeft, &input);
	
	Dispatcher.queue_event(ctx, ei);
	Assert.isTrue(List.count(ctx->events) == 1, "event count mismatch");
	
	event_info actInfo = List.getAt(ctx->events, 0);
	Assert.isTrue(actInfo != NULL, "ctx->events[0] not retrieved");
	event e = actInfo->e;
	Assert.areEqual(&expMouseX, &e->data.mouse.x, INT, "mouse_x mismatch");
	Assert.areEqual(&expMouseY, &e->data.mouse.y, INT, "mouse_y mismatch");
	Assert.areEqual(&expMouseLeft, &e->data.mouse.button, INT, "mouse_button mismatch");
	flogf(stdout, "ei: type=%d mouse_x=%d mouse_y=%d mouse_button=%d", e->type, e->data.mouse.x, e->data.mouse.y, e->data.mouse.button);

	Sigui.free_context(ctx);
}
/* test event dispatch */
void dispatch_queued_event(void) {
	printf("\n");
	fflush(stdout);
	
	ui_context ctx = Sigui.new_context(NULL);
	window win = Sigui.new_window(0, 0, 100, 200);

	ui_module m = Sigui.add_module(ctx, "TestModule", dummy_render, test_handler, win);
	m->enabled = 1;

	ui_input input = {0};
	event_info ei = create_mouse_event(EVENT_MOUSE_PRESS, 10, 20, 1, &input);
	Dispatcher.queue_event(ctx, ei);

	event_handled = 0;
	Dispatcher.dispatch_events(ctx);
	Assert.isTrue(event_handled == 1, "Event handler should be called");
	Assert.isTrue(List.count(ctx->events) == 0, "Event queue should be cleared");

	Sigui.free_context(ctx);
}
/* test command queueing */
void queue_ui_command(void) {
	printf("\n");
	fflush(stdout);
	
	ui_context ctx = Sigui.new_context(NULL);
	Assert.isTrue(ctx != NULL, "context should not be NULL");
	
	command cmd = Sigui.new_command("test");
	cmd->execute = test_command_execute;

	Dispatcher.queue_command(ctx, cmd);
	Assert.isTrue(List.count(ctx->commands) == 1, "Command should be queued");

	Sigui.free_context(ctx);
}
/* test command dispatch */
static void dispatch_queued_command(void) {
	printf("\n");
	fflush(stdout);
	
	flogf(stdout, "creating ui context");
	ui_context ctx = Sigui.new_context(NULL);
	window win = Sigui.new_window(0, 0, 100, 200);
	ui_module m = Sigui.add_module(ctx, "TestModule", dummy_render, test_command_handler, win);
	m->enabled = 1;
	flogf(stdout, "created module=%s", m->name);

	ui_input input = {0};
	event_info ei = create_mouse_event(EVENT_MOUSE_PRESS, 10, 20, 1, &input);
	Dispatcher.queue_event(ctx, ei);

	flogf(stdout, "dispatching events");
	Dispatcher.dispatch_events(ctx);
	Dispatcher.dispatch_commands(ctx);
	Assert.isTrue(List.count(ctx->commands) == 0, "Commands should be cleared");

	flogf(stdout, "dispatching events");
	Dispatcher.dispatch_events(ctx);
	Dispatcher.dispatch_commands(ctx);
	Assert.isTrue(List.count(ctx->commands) == 0, "Commands should be cleared");
	
	Sigui.free_context(ctx);
}
/* test input transitions */
static void validate_input_transitions(void) {
	printf("\n[TEST] Starting Input Transitions Test\n");
	fflush(stdout);
	
	reset_event_counts();
	
	ui_context ctx = Sigui.new_context(NULL);
	window win = Sigui.new_window(50, 50, 300, 400);
	Sigui.add_module(ctx, "TestWindow", dummy_render, test_transition_handler, win);
	
	ui_input input = {0};
	const int frames = 5;
	int i = 0;
	while (i < frames) {
		flogf(stdout, "<Frame=%d>", i);
		
		input.mouse_x = i * 10;
		input.mouse_y = i * 20;
		input.button = (i % 2) == 0 ? MOUSE_BUTTON_LEFT : MOUSE_BUTTON_NONE;
		input.keys['A'] = (i == 3);
		
		Sigui.render(ctx, &input);
		
		++i;
	}
	
	//	verify expected events
	Assert.isTrue(event_counts[EVENT_MOUSE_PRESS] == 3, "should have 3 mouse press events: 0->1, 2->3, 4->5");
	Assert.isTrue(event_counts[EVENT_MOUSE_RELEASE] == 2, "should have 2 mouse release events: 1->2, 3->4");
	Assert.isTrue(event_counts[EVENT_KEY_PRESS] == 1, "should have 1 key press event: 'space' at 3");
	Assert.isTrue(event_counts[EVENT_KEY_RELEASE] == 1, "should have 1 key release event: 'space' at 4");
	
	Sigui.free_context(ctx);
}

static void dummy_render(ui_context ctx, ui_module module, ui_input* input) {
	//	no-op dummy renderer ...
}
static void test_handler(ui_context ctx, ui_module module, event_info ei) {
	if (!ei) Assert.isTrue(0, "event_handler did not get event_info");
	event e = ei->e;
	
	if (e->type == EVENT_MOUSE_PRESS) {
		event_handled = 1;
		Assert.isTrue(e->data.mouse.x == 10 && e->data.mouse.y == 20, "event data mismatch");
	}
	flogf(stdout, "ei: type=%d mouse_x=%d mouse_y=%d mouse_button=%d", e->type, e->data.mouse.x, e->data.mouse.y, e->data.mouse.button);
}
static void test_transition_handler(ui_context ctx, ui_module module, event_info ei) {
	if (!ei) Assert.isTrue(0, "event_handler did not get event_info");
	event e = ei->e;
	flogf(stdout, "handling event: type=%d", e->type);
	event_counts[e->type]++;	// count event types
	
	
	if (e->type == EVENT_MOUSE_PRESS) {
		command cmd = Sigui.new_command("click: show_message");
		cmd->target = module; 						// the target module
		cmd->execute = test_command_execute;
		
		flogf(stdout, "enqueueing command name=%s target=%s", cmd->name, module->name);
		Dispatcher.queue_command(ctx, cmd);
	} else if (e->type == EVENT_KEY_PRESS && e->data.key.key_code == ' ') {
		command cmd = Sigui.new_command("key: toggle_state");
		cmd->target = module; 						// the target module
		cmd->execute = test_command_execute;
		
		flogf(stdout, "enqueueing command name=%s target=%s", cmd->name, module->name);
		Dispatcher.queue_command(ctx, cmd);
	}
}
static void test_command_execute(ui_context ctx, ui_module module) {
	printf("   <Command> executed module=%s\n", module->name);
}
static void test_command_handler(ui_context ctx, ui_module module, event_info ei) {
	if (!ei) Assert.isTrue(0, "event_handler did not get event_info");
	event e = ei->e;
	
	flogf(stdout, "building command: event=%d", e->type);
	if (e->type == EVENT_MOUSE_PRESS) {
		command cmd = Sigui.new_command("test_cmd");
		cmd->target = module; 						// the target module
		cmd->execute = test_command_execute;
		
		flogf(stdout, "enqueueing command name=%s target=%s", cmd->name, module->name);
		Dispatcher.queue_command(ctx, cmd);
	}
}
static event_info create_mouse_event(event_type type, int mouse_x, int mouse_y, int button, ui_input* input) {
	input->mouse_x = mouse_x;
	input->mouse_y = mouse_y;
	input->button = button;
	//	reset keys
	memset(input->keys, 0, sizeof(input->keys));
	
	return Sigui.new_event(type, input, button);
}
static event_info create_keyboard_event(event_type type, int key, ui_input* input) {
	input->mouse_x = 0;
	input->mouse_y = 0;
	input->button = 0;
	//	reset keys
	memset(input->keys, 0, sizeof(input->keys));
	input->keys[key] = 1;
	
	return Sigui.new_event(type, input, key);
}
static void reset_event_counts(void) {
	//	reset event counts
	for (int i = 0; i <= EVENT_KEY_RELEASE; i++) event_counts[i] = 0;
}

// Register test cases
__attribute__((constructor)) void init_sigtest_tests(void) {
	register_test("unit_testing", unit_testing);
	register_test("queue_ui_event", queue_ui_event);
	register_test("dispatch_queued_event", dispatch_queued_event);
	register_test("queue_ui_command", queue_ui_command);
	register_test("dispatch_queued_command", dispatch_queued_command);
	register_test("validate_input_transitions", validate_input_transitions);
}
