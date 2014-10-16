#include "scheduler.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

list_node *SCHEDULED_EVENTS;

void __api_schedule(solid_vm *vm)
{
	int ticks = solid_get_int_value(solid_pop_stack(vm));
	solid_object *cb = solid_pop_stack(vm);
	schedule(make_thunk_solid(cb), ticks);
}

void initialize_scheduler()
{
	defun("schedule", __api_schedule);
	SCHEDULED_EVENTS = make_list();
}

void schedule(thunk cb, int ticks)
{
	scheduled_event *e = (scheduled_event *) malloc(sizeof(scheduled_event));
	e->cb = cb;
	e->ticks_remaining = ticks;
	insert_list(SCHEDULED_EVENTS, e);
}

void update_scheduler()
{
	list_node *c;
	list_node *temp;
	scheduled_event *e;
	for (c = SCHEDULED_EVENTS->next; c->next != NULL; c = c->next) {
		if (((scheduled_event *) c->data) != NULL) {
			e = (scheduled_event *) c->data;
			if (e->ticks_remaining <= 0) {
				execute_thunk(e->cb);
				temp = c->prev;
				c->prev->next = c->next;
				c->next->prev = c->prev;
				c = temp;
				free(e);
			} else {
				--(e->ticks_remaining);
			}
		}
	}
}
