#pragma once

#include <solid/solid.h>

#include "utils.h"

typedef struct effect {
	double x, y;
	int lifespan;
	solid_object *update;
} effect;

typedef struct particle {
	color c;
	double x, y;
	double w, h;
	int lifespan;
	solid_object *update;
} particle;

typedef struct global_effect {
	void (*callback)();
	int lifespan;
} global_effect;

void initialize_fx();
void reset_fx();

effect *build_fx_prototype(char *name, int lifespan, solid_object *update);

effect *make_fx(char *name, double x, double y);

void spawn_fx(effect *e);
void spawn_particle(color c, double x, double y, double w, double h, int lifespan, solid_object *update);

global_effect *make_global_fx(void (*callback)(), int lifespan);

void global_effect_shake();
void spawn_global_fx(global_effect *e);

void update_fx();
void apply_global_fx();

void draw_fx();
