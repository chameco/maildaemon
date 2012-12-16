#ifndef FX_H
#define FX_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#define SPAWN_SMOKE_CONST(x, y, radius) \
	spawn_fx(SMOKE_CONST, COLOR_GRAY, \
				x, y, 8, radius, 1)
typedef struct effect {
	color c;
	int x, y, dim;
	int radius;
	int speed;
	int cur;
	int statelen;
	int state[25][3];
	etype type;
} effect;
void initialize_fx();
void spawn_fx(etype type, color col,
		int x, int y, int dim,
		int radius, int speed);
void update_fx();
void draw_particle(effect *e, int xdiff, int ydiff);
void draw_smoke_particle(effect *e, int xdiff, int ydiff);
void draw_one_fx(effect *e);
void draw_fx();
#endif
