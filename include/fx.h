#pragma once

#include "utils.h"

typedef enum etype {
	EXPLOSION,
	SMOKE_CONST,
	SMOKE
} etype;

typedef struct effect {
	color c;
	int x, y, dim;
	int radius;
	int speed;
	int cur;
	int statelen;
	int state[30][5]; //x, y, xv, yv, exists? for each particle
	etype type;
} effect;

typedef struct global_effect {
	void (*callback)();
	int timer;
} global_effect;

void initialize_fx();
void reset_fx();

effect *make_fx(etype type, color col,
		int x, int y, int dim,
		int radius, int speed);
void spawn_fx(effect *e);

global_effect *make_global_fx(void (*callback)(), int timer);

void global_effect_shake();
void spawn_global_fx(global_effect *e);

void update_fx();
void apply_global_fx();

void draw_particle(effect *e, int xdiff, int ydiff);
void draw_smoke_particle(effect *e, int xdiff, int ydiff);
void draw_one_fx(effect *e);
void draw_fx();
