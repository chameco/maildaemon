#ifndef PURGE_FX_H
#define PURGE_FX_H
#include "utils.h"

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
void initialize_fx();
void reset_fx();
void spawn_fx(etype type, color col,
		int x, int y, int dim,
		int radius, int speed);
void update_fx();
void draw_particle(effect *e, int xdiff, int ydiff);
void draw_smoke_particle(effect *e, int xdiff, int ydiff);
void draw_one_fx(effect *e);
void draw_fx();
#endif
