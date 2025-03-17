// test_inputs.c
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

const string mouse_buttons[] = {"MOUSE_BUTTON_NONE", "MOUSE_BUTTON_LEFT", "MOUSE_BUTTON_RIGHT", "MOUSE_BUTTON_LEFT+MOUSE_BUTTON_RIGHT"};

static int event_counts[EVENT_KEY_RELEASE + 1] = {0};
static int mouse_input_mask[] = {0, 0, 0, 0};
static int event_id = 0;

//	dummy renderer function for module
static void dummy_render(ui_context, ui_module, ui_input*);
//	input test handler
static void input_test_handler(ui_context, ui_module, event_info);

//	set up context
static ui_context set_up_context(void);
//	clean up context
static void clean_up_context(ui_context);
//	create a mouse event_info object
static event_info create_mouse_event(event_type, int, int, int, ui_input*);
//	create a keyboard event_info object
static event_info create_keyboard_event(event_type, int, ui_input*);
//	reset event counts
static void reset_event_counts(void);

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
	
	flogf(stdout, "Test Run Date=%s", date_str);
}
/* test multi-button detection */
void multi_button_input(void) {
	/* handler should confirm 2 presses, 2 release and the buttons associated
	 *		- Frame 0: EVENT_MOUSE_PRESS (left).
	 *		- Frame 1: EVENT_MOUSE_PRESS (left), EVENT_MOUSE_PRESS (right).
	 *		- Frame 1: EVENT_MOUSE_RELEASE (left).
	 *		- Frame 2: EVENT_MOUSE_RELEASE (right).
	 */
	printf("\n");
	fflush(stdout);
	
	ui_context ctx = set_up_context();
	reset_event_counts();
	ui_input input = {0};
	
	//	frame 0: left pressed
	input.button = MOUSE_BUTTON_LEFT;
	input.mouse_x = 10; input.mouse_y = 20;
	Sigui.render(ctx, &input);
	
	// frame 1: left + right presssed
	input.button = MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT;
	Sigui.render(ctx, &input);
	
	//	frame 2: left release, right pressed
	input.button = MOUSE_BUTTON_RIGHT;
	Sigui.render(ctx, &input);
	
	//	frame 3: right (all) released
	input.button = MOUSE_BUTTON_NONE;
	Sigui.render(ctx, &input);
	
	//	verify
	Assert.isTrue(event_counts[EVENT_MOUSE_PRESS] == 2, "expected 2 mouse press events");
	Assert.isTrue(event_counts[EVENT_MOUSE_RELEASE] == 2, "expected 2 mouse release events");
	Assert.isTrue(event_counts[EVENT_KEY_PRESS] == 0, "expected 0 keyboard events");
	
	Assert.isTrue(mouse_input_mask[0] == MOUSE_BUTTON_LEFT, "[0] expected MOUSE_INPUT_MASK");
	Assert.isTrue(mouse_input_mask[1] == MOUSE_BUTTON_RIGHT, "[1] expected MOUSE_BUTTON_RIGHT");
	Assert.isTrue(mouse_input_mask[2] == MOUSE_BUTTON_LEFT, "[2] expected MOUSE_BUTTON_LEFT");
	Assert.isTrue(mouse_input_mask[3] == MOUSE_BUTTON_RIGHT, "[3] expected MOUSE_BUTTON_RIGHT");
	
	clean_up_context(ctx);
}
/* test multi-key detection */
static void multi_key_input(void) {
	/* handler should confirm 2 presses, 2 release and the buttons associated
	 *		- Frame 0: EVENT_KEY_PRESS ('A').
	 *		- Frame 1: EVENT_KEY_PRESS ('A'), EVENT_KEY_PRESS ('W').
	 *		- Frame 1: EVENT_KEY_RELEASE ('A').
	 */
	printf("\n");
	fflush(stdout);
	
	ui_context ctx = set_up_context();
	reset_event_counts();
	ui_input input = {0};
	
	//	frame 0: 'A' pressed
	input.keys['A'] = 1;
	Sigui.render(ctx, &input);
	
	//	frame 1: 'A' + 'W' pressed
	input.keys['W'] = 1;
	Sigui.render(ctx, &input);
	
	//	frame 2: 'A' release
	input.keys['A'] = 0;
	Sigui.render(ctx, &input);
	
	//	verify
	Assert.isTrue(event_counts[EVENT_KEY_PRESS] == 2, "expected 2 key press events");
	Assert.isTrue(event_counts[EVENT_KEY_RELEASE] == 1, "expected 1 key release events");
	Assert.isTrue(event_counts[EVENT_MOUSE_PRESS] == 0, "expected 0 mouse events");
	
	clean_up_context(ctx);
}

//	Handlers ====================================================================
static void dummy_render(ui_context ctx, ui_module module, ui_input* input) {
	//	no-op dummy renderer ...
}
static void input_test_handler(ui_context ctx, ui_module module, event_info ei) {
	if (!ei) Assert.isTrue(0, "input test handler did not get event_info");
	event e = ei->e;
	
	flogf(stdout, "handling event: type=%d", e->type);
	event_counts[e->type]++;			// increment event type count
	switch (e->type) {
		case EVENT_MOUSE_PRESS:
			mouse_input_mask[event_id] = e->data.mouse.button;
			flogf(stdout, "[id=%d] mouse press: button=%d", event_id, e->data.mouse.button);
			
			break;
		case EVENT_MOUSE_RELEASE:
			mouse_input_mask[event_id] = e->data.mouse.button;
			flogf(stdout, "[id=%d] mouse release: button=%d", event_id, e->data.mouse.button);
			
			break;
		case EVENT_KEY_PRESS:
			//	handle the key press
			flogf(stdout, "[id=%d] key press: code=%d", event_id, e->data.key.key_code);
			
			break;
		case EVENT_KEY_RELEASE:
			//	handle the key press
			flogf(stdout, "[id=%d] key release: code=%d", event_id, e->data.key.key_code);
			
			break;
		default:
			flogf(stdout, "[id=%d] invalid event: type=%d", event_id, e->type);
			Assert.isTrue(0, "input event type mismatch");
			break;
	}
	
	++event_id;
}

//	Sigui Test Functions ========================================================
//	[TODO] TASK: (maintenance, low priority) Move to `sigui_test.h`
static ui_context set_up_context(void) {
	ui_context ctx = Sigui.new_context(NULL);
	window win = Sigui.new_window(0, 0, 100, 200);
	Sigui.add_module(ctx, "TestModule", dummy_render, input_test_handler, win);
	
	return ctx;
}
static void clean_up_context(ui_context ctx) {
	Sigui.free_context(ctx);
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
	event_id = 0;
	memset(event_counts, 0, sizeof(event_counts));
}

// Register test cases
__attribute__((constructor)) void init_sigtest_tests(void) {
    register_test("test_harness", test_harness);
//    register_test("multi_button_input", multi_button_input);
    register_test("multi_key_input", multi_key_input);
}
