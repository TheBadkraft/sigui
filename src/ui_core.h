// ui_core.h
#ifndef UI_CORE_H
#define UI_CORE_H

#include "sigui.h"

// Helper Functions ============================================================


/* opaque sigui module structure */
struct sigui_module_s {
	string name;				/* module name */
	ui_render render;			/* delegate renderer */
	event_handler handler;	/* event delegate */
	int enabled;				/* enabled flag (1=TRUE, 0=FALSE) */
	window win;					/* module window */
}; 								// ui_module
/* opaque sigui context structure */
struct sigui_context_s {
	list modules;				/* context modules */
	list events;				/* context event queue */
	list commands;				/* context command queue */
	object state;				/* user-defined state */
	ui_input input_state;	/* last input state */
};									// ui_context


#endif	//	UI_CORE_H
