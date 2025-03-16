// dispatcher.c
/** 
 * @detail Here we can add a lot of detail about what is going on from a 30,000 foot perspective. 
 */
 
#include "sigui.h"
#include "ui_core.h"
 
/* enqueue an event to the context queue */
static void enqueue_event(ui_context ctx, event_info ei) {
	if (!ctx || !ei) return;
	
	List.add(ctx->events, ei);
	printf("   <Dispatch> equeued event\n");
}
/* dispatches context events */
static void dispatch_events(ui_context ctx) {
	if (!ctx || !ctx->events || List.count(ctx->events) == 0) return;
	
	iterator e_it = Array.getIterator(ctx->events, LIST);
	while (Iterator.hasNext(e_it)) {
		event_info ei = Iterator.next(e_it);
		iterator m_it = Array.getIterator(ctx->modules, LIST);
		while (Iterator.hasNext(m_it)) {
			ui_module m = Iterator.next(m_it);
			if (m->enabled && m->handler) {
				printf("   <Dispatch> event module=%s\n", m->name); 
				m->handler(ctx, m, ei);
			}
		}
		Iterator.free(m_it);
		Mem.free(ei->e);
		Mem.free(ei);
	}
	
	Iterator.free(e_it);
	List.clear(ctx->events);
}
/* enqueue a command to the context queue */
static void enqueue_command(ui_context ctx, command c) {
	if (!ctx || !c) return;
	
	List.add(ctx->commands, c);
	printf("   <Dispatch> enqueued command\n");
}
/* dispatches context commands */
static void dispatch_commands(ui_context ctx) {
	printf("   <Dispatch> begin\n");
	fflush(stdout);
	if (!ctx || !ctx->commands || List.count(ctx->commands) == 0) return;
	
	iterator it = Array.getIterator(ctx->commands, LIST);
	printf("   <Dispatch> hasIterator=%s\n", it ? "TRUE" : "FALSE");
	fflush(stdout);
	
	while (Iterator.hasNext(it)) {
		command c = Iterator.next(it);
		printf("   <Dispatch> hasCommand=%s\n", c ? c->name : "FALSE");
		fflush(stdout);
		
		if (c->execute) {
			printf("   <Dispatch> command=%s valid=%s target=%s\n", c->name,
					  	c->execute ? "TRUE" : "FALSE", 
					  	c->target->name ? c->target->name : "NULL");
			fflush(stdout);
			
			c->execute(ctx, c->target);
		}
		if (c->name) String.free(c->name);
		Mem.free(c);
	}
	
	Iterator.free(it);
	List.clear(ctx->commands);
	printf("   <Dispatch> end\n");
	fflush(stdout);
}

/* dispatcher interface */
const IDispatcher Dispatcher = {
	.queue_event = enqueue_event,
	.dispatch_events = dispatch_events,
	.queue_command = enqueue_command,
	.dispatch_commands = dispatch_commands
};
