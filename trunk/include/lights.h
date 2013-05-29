#ifndef LIGHTS_H
#define LIGHTS_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "level.h"
#include "utils.h"

typedef struct lightsource {
	int x, y, dim, intensity;
	color c;
} lightsource;

void initialize_lights();

void reset_lights();

lightsource *create_lightsource(int x, int y, int dim, int intensity, color c);

void trace_lightsource(lightsource *l);

void spawn_lightsource(int x, int y, int dim, int intensity, color c);

void draw_lights();

#endif
