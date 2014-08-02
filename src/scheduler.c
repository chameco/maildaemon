#include "scheduler.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

list_node *SCHEDULED_EVENTS;

SCM __api_schedule(SCM cb, SCM ticks)
{
	schedule(make_thunk_scm(cb), scm_to_int(ticks));
	return SCM_BOOL_F;
}

void initialize_scheduler()
{
	scm_c_define_gsubr("schedule", 2, 0, 0, __api_schedule);
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
