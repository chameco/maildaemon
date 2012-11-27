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
#define spawn_smoke_const(x, y, radius) \
	spawn_effect(SMOKE_CONST, 0.1, 0.1, 0.1, 1.0, \
				x, y, 8, radius, 1)
typedef struct effect {
	float r, g, b, a;
	int x, y, dim;
	int radius;
	int speed;
	int cur;
	int statelen;
	int state[25][3];
	etype type;
} effect;
void initialize_effects();
void spawn_effect(etype type, float r, float g, float b, float a,
		int x, int y, int dim,
		int radius, int speed);
void update_effects();
void draw_particle(effect *e, int xdiff, int ydiff);
void draw_smoke_particle(effect *e, int xdiff, int ydiff);
void draw_effect(effect *e);
void draw_effects();
#endif
