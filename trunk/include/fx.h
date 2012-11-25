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
typedef struct effect {
	float r, g, b, a;
	int x, y, dim;
	int radius;
	int speed;
	int cur;
} effect;
void initialize_effects();
void spawn_effect(float r, float g, float b, float a,
		int x, int y, int dim,
		int radius, int speed);
void update_effects();
void draw_particle(effect *e, int xdiff, int ydiff);
void draw_effect(effect *e);
void draw_effects();
#endif
