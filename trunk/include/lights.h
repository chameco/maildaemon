#ifndef PURGE_LIGHTS_H
#define PURGE_LIGHTS_H
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
