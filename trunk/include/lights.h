#ifndef PURGE_LIGHTS_H
#define PURGE_LIGHTS_H
#include "utils.h"

typedef struct lightsource {
	int x, y, dim, intensity;
	color c;
	int freeable;
} lightsource;

void initialize_lights();

void reset_lights();

lightsource *make_lightsource(int x, int y, int dim, int intensity, color c);
void spawn_lightsource(lightsource *l);

void draw_lights();

#endif
