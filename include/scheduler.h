#pragma once

#include "utils.h"

typedef struct scheduled_event {
	thunk cb;
	int ticks_remaining;
} scheduled_event;

void initialize_scheduler();

void schedule(thunk cb, int ticks);

void update_scheduler();
