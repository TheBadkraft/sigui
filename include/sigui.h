// sigui.h
#ifndef SIGUI_H
#define SIGUI_H

#include <sigcore.h>
#include <stdio.h>

//	Forward Declarations ========================================================
/** @brief Opaque pointer to a sigui context */
typedef struct sigui_context_s* ui_context;
/** @brief Opaque poitner to a sigui module */
typedef struct sigui_module_s* ui_module;

//	Types =======================================================================
/** #brief Event types for the UI system. */
typedef enum {
	EVENT_MOUSE_PRESS,
	EVENT_MOUSE_RELEASE,
	EVENT_MOUSE_MOVE,
	EVENT_MOUSE_SCROLL,
	EVENT_KEY_PRESS,
	EVENT_KEY_RELEASE
} event_type;
/** @breif Mouse buttons */
typedef enum {
	//	[TODO] TASK: use to create button masking for multiple buttons
	MOUSE_BUTTON_NONE = 0,				// 0
	MOUSE_BUTTON_LEFT = 1 << 0,		// 1
	MOUSE_BUTTON_RIGHT = 1 << 1,		// 2
	MOUSE_BUTTON_CENTER = 1 << 2,		// 4
	MOUSE_BUTTON_4 = 1 << 3,			// 8
	MOUSE_BUTTON_5 = 1 << 4,			// 16
	MOSUE_BUTTON_6 = 1 << 5,			// 32
} mouse_button;
/** @brief Input state for mouse and keyboard */
typedef struct ui_input_s {
	int mouse_x, mouse_y;		/**< Mouse position. */
	uint32_t button;				/**< Mouse button mask */
	//	[TODO] TASK: update keyboard input similar to mouse updates
	uint8_t keys[256];			/**< key states: 1 = pressed; 0 = released */
} ui_input;
struct input_state_s {
	ui_input* state;
};
typedef struct input_state_s* input_state;
/** @brief Window properties (public - heap allocated) */
struct ui_window_s {
	int x, y;						/**< Position: x, y */
	int width, height;			/**< Size: width, height */
};
typedef struct ui_window_s* window;
/** @brief Event structure */
struct event_s {
	event_type type;				/**< Type of event */
	union {
		struct {
			int x, y;				/**< Mouse coordinates (if applicable) */
			uint32_t button;		/**< Mouse button id */
		} mouse;						/**< Data for mouse events */
		struct {
			int key_code;			/**< Key code (if applicable) */
		} key;						/**< Data for keyboard events */
	} data;							/**< Union of event specific data */
};
typedef struct event_s* event;
/** @brief Event info for extensibility */
struct event_info_s {
	event e;			/**< The event data */
	//	extend via ui_module, target, timestamp, etc.
};
typedef struct event_info_s* event_info;
/** @brief Command structure for actionable responses */
struct command_s {
	string name;										/**< Command identifier (e.g. "open_menu") */
	ui_module target;									/**< Target module for command execution */
	void (*execute)(ui_context, ui_module);	/**< Function delegate to executethe command */
};
typedef struct command_s* command;

//	Delegates ===================================================================
/** @brief Render delegate function for modules */
typedef void (*ui_render)(ui_context, ui_module, ui_input*);
/** brief Event handler delegate */
typedef void (*event_handler)(ui_context, ui_module, event_info);

//	Interfaces ==================================================================
/**
 * @brief Interface for managing a structured immediate mode UI.
 * @details Provides methods to intiialize , add modules, render, and clean up a sigui context.
 */
typedef struct ISigui {
	ui_context (*new_context)(object);					/**< Creates a new sigui context with user-defined state. */
	void (*free_context)(ui_context);					/**< Frees the sigui context and its resources */
	window (*new_window)(int, int, int, int);			/**< Create a new window. */
	ui_module (*add_module)(ui_context, string, 		/**< Adds a module with a name and render function */
							 		ui_render, event_handler, window);
	void (*render)(ui_context, ui_input*);				/**< Renders all enabled modules with input */
	event_info (*new_event)(event_type, ui_input*,	/**< Create a new event */
									uint32_t);
	command (*new_command)(const string);				/**< Create a new command */
} ISigui;
/**
 * @brief Interface for the event queuing and dispatching
 */
 typedef struct IDispatcher {
 	void (*queue_event)(ui_context, event_info);	/**< Queue an event in the contexts event queue */
 	void (*dispatch_events)(ui_context);			/**< Dispatches all the context's events */
 	void (*queue_command)(ui_context, command);	/**< Queue a command in the contexts command queue */
 	void (*dispatch_commands)(ui_context);			/**< Dispatches all the context's commands */
 } IDispatcher;
 
extern const ISigui Sigui;							/**< Global Sigui interface instance */
extern const IDispatcher Dispatcher;			/**< Global Dispatcher interface instance */

#endif // SIGUI_H
